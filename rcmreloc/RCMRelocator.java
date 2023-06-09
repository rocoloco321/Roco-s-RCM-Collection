import java.io.*;
import java.nio.file.*;
import java.util.*;

public class RCMRelocator {

	public static String[] readLines(String path) {
		try {
			byte[] b = Files.readAllBytes(Paths.get(path));
			String asStr = new String(b);
			
			//replace \r\n with \n for simplicity
			asStr = asStr.replaceAll("\r\n", "\n");
			asStr = asStr.replaceAll("\r", "\n");
			return asStr.split("\n");
			
		} catch(IOException ex) {
			return null;
		}
	}
	
	public static String getSymType(String[] syms, String sym) {
		for(String s : syms) {
			try {
				String sansAddress = s.substring(9); //8-char address+space
				String type = sansAddress.substring(0, 7);
				String sectionSizeName = sansAddress.substring(8);
				sectionSizeName = sectionSizeName.replaceAll("\\s+", " ").trim();
				String[] fields = sectionSizeName.split(" ");
				String section = fields[0];
				String name = fields[2];
				if(!section.equals("*ABS*") && !section.equals("*UND*") && name.equals(sym))
					return type;
			} catch(Exception ex) {
				//invalid line, just skip it
			}
		}
		return null;
	}
	
	public static int getSymAddress(String[] syms, String sym) {
		for(String s : syms) {
			try {
				String sectionSizeName = s.substring(17);
				sectionSizeName = sectionSizeName.replaceAll("\\s+", " ").trim();
				String[] fields = sectionSizeName.split(" ");
				String section = fields[0];
				String name = fields[2];
				String strAddr = s.substring(0, 8);
				int intAddr = Integer.parseInt(strAddr, 16);
				if(!section.equals("*ABS*") && !section.equals("*UND*") && name.equals(sym))
					return intAddr;
			} catch(Exception ex) {
				//invalid line, just skip it
			}
		}
		return 0;
	}
	
	public static boolean getSymExists(String[] syms, String sym) {
		return getSymType(syms, sym) != null;
	}

	public static void main(String[] args) {
		//read arguments. First RCM name, then relocation output from objdump, then symbol output
		//from objdump
		
		if(args.length < 3) {
			System.err.println("[ERROR] Usage: java RCMRelocator <RCM File> <relocations> <symbols>");
			System.exit(1);
		}
		
		String rcmPath = args[0];
		String relocPath = args[1];
		String symPath = args[2];
		
		String[] relocs = readLines(relocPath);
		String[] syms = readLines(symPath);
		
		if(relocs == null || syms == null) {
			System.err.println("[ERROR] File not found.");
			System.exit(1);
		}
		
		List<Relocation> relocations = new ArrayList<>();
		
		//next up, process relocations in order
		for(String s : relocs) {
			try {
				String entry = s.replaceAll("\\s+", " ").trim();
				String[] info = entry.split(" ");
				
				String strOffset = info[0];
				String strInfo = info[1];
				String strType = info[2];
				String strValue = info[3];
				String strName = info[4];
				int intOffset = Integer.parseInt(strOffset, 16);
				int intInfo = Integer.parseInt(strInfo, 16);
				int intValue = Integer.parseInt(strValue, 16);
				
				relocations.add(new Relocation(intOffset, strType, intValue, getSymExists(syms, strName), strName));
			} catch(Exception ex) {
				//invalid line, discard
			}
		}
		
		//print relocations
		for(Relocation r : relocations) {
			System.out.println(r);
		}
		
		//generate relocation section
		try {
			byte[] rcm = Files.readAllBytes(Paths.get(rcmPath));
			ByteArrayOutputStream baos = new ByteArrayOutputStream();	//relocation table
			ByteArrayOutputStream thunks = new ByteArrayOutputStream();	//thunks (for ARM->THUMB B)
			try {
				
				//first pass: normal relocations. Only output actual relocations.
				boolean errUnresolved = false;
				for(Relocation r : relocations) {
					if(r.type.equals("R_ARM_V4BX")) continue; //ignore these
					
					//check: is destination THUMB, and is the instruction a B?
					//or: is it a Bxx that can be resolved without a relocation?
					boolean isThumb = (r.value & 1) == 1;
					boolean isB = r.type.equals("R_ARM_JUMP24");
					boolean isRelative = r.type.equals("R_ARM_JUMP24") || r.type.equals("R_ARM_CALL");
					boolean isLocal = r.local;
					if(!(isThumb && isB) && !(isRelative && isLocal)) {
						if(!(!isLocal && r.value == 0)) { //check external relocations for NULL
							baos.write(r.getBytes()); //write relocation as normal
						} else {
							//external relocation points to NULL
							//probably means a symbol was not resolved, throw an error
							System.err.println("[ERROR] Unresolved external symbol " + r.name);
							errUnresolved = true;
						}
					}
				}
				if(errUnresolved) {
					System.exit(1); //only exit after showing all symbol errors
				}
				
				//second pass: resolve resolvable relocations
				int thunkOffset = rcm.length + baos.size();
				for(Relocation r : relocations) {
					if(r.type.equals("R_ARM_V4BX")) continue; //ignore these
					
					//check: is destination THUMB, and is the instruction a B? If so, create a thunk
					boolean isThumb = (r.value & 1) == 1;
					boolean isB = r.type.equals("R_ARM_JUMP24");
					boolean isRelative = r.type.equals("R_ARM_JUMP24") || r.type.equals("R_ARM_CALL");
					boolean isLocal = r.local;
					if(isThumb && isB) {
						System.out.println("[DEBUG] --------------WRITING THUMB THUNK AT " + String.format("%08X", r.offset));
						
						//create thunk:  LDR PC, =(addr)
						int currentOffset = thunks.size();
						int dest = r.value;
						byte[] thunk = {
							(byte) 0x04, (byte) 0xF0, (byte) 0x1F, (byte) 0xE5,
							(byte) (dest & 0xFF),
							(byte) ((dest >>> 8) & 0xFF),
							(byte) ((dest >>> 16) & 0xFF),
							(byte) ((dest >>> 24) & 0xFF)
						};
						thunks.write(thunk);
						
						//overwrite original instruction
						int rel = (currentOffset + thunkOffset - (r.offset + 8)) >>> 2;
						byte[] newBranch = {
							(byte) (rel & 0xFF),
							(byte) ((rel >>> 8) & 0xFF),
							(byte) ((rel >>> 16) & 0xFF),
							(byte) 0xEA
						};
						rcm[r.offset + 0] = newBranch[0];
						rcm[r.offset + 1] = newBranch[1];
						rcm[r.offset + 2] = newBranch[2];
						rcm[r.offset + 3] = newBranch[3];
					} else if(isRelative && isLocal) {
						System.out.println("[DEBUG] --------------RESOLVING LOCAL BRANCH AT " + String.format("%08X", r.offset));
						
						int instrOffset = r.offset;
						int instr = (rcm[r.offset] & 0xFF) | ((rcm[r.offset + 1] & 0xFF) << 8)
							| ((rcm[r.offset + 2] & 0xFF) << 16)
							| ((rcm[r.offset + 3] & 0xFF) << 24);
						
						int offset = (instr & 0x00FFFFFF) << 2;
						if((offset & 0x02000000) != 0) offset -= 0x04000000;
						offset = ((offset + r.value - r.offset) >>> 2) & 0x00FFFFFF;
						instr = (instr & 0xFF000000) | offset;
						
						byte[] newBranch = {
							(byte) (instr & 0xFF),
							(byte) ((instr >>> 8) & 0xFF),
							(byte) ((instr >>> 16) & 0xFF),
							(byte) ((instr >>> 24) & 0xFF)
						};
						rcm[r.offset + 0] = newBranch[0];
						rcm[r.offset + 1] = newBranch[1];
						rcm[r.offset + 2] = newBranch[2];
						rcm[r.offset + 3] = newBranch[3];
					}
				}
			} catch(IOException ex) {
				throw new IllegalStateException("What?");
			}
			byte[] relocBytes = baos.toByteArray();
			byte[] thunkBytes = thunks.toByteArray();
			
			//write relocation data
			int ofsReloc = rcm.length;
			int szReloc = relocBytes.length >>> 3;
			rcm[4] = (byte) (ofsReloc & 0xFF);
			rcm[5] = (byte) ((ofsReloc >>> 8) & 0xFF);
			rcm[6] = (byte) (szReloc & 0xFF);
			rcm[7] = (byte) ((szReloc >>> 8) & 0xFF);
			
			File f = new File(rcmPath);
			OutputStream out = new FileOutputStream(f);
			out.write(rcm);
			out.write(relocBytes);
			out.write(thunkBytes);
			out.close();
			
		} catch(IOException ex) {
			ex.printStackTrace();
			System.exit(1);
		}
		
	}
	
}

class Relocation {

	public static int relocTypeFromString(String type, boolean local) {
		//if local, make the type relative
		if(local) {
			switch(type) {
				case "R_ARM_ABS32":
					type = "R_ARM_BASE_ABS"; break;
				case "R_ARM_CALL":
					break;
				default:
					throw new IllegalStateException("Unknown relative relocation type " + type);
			}
		}
		
		switch(type) {
			case "R_ARM_CALL":
				return 28;
			case "R_ARM_ABS32":
				return 2;
			case "R_ARM_JUMP24":
				return 29;
			case "R_ARM_V4BX":
				return 40;
			case "R_ARM_BASE_ABS":
				return 31;
			default:
				throw new IllegalStateException("Unknown relocation type " + type);
		}
	}
	
	public int offset;
	public String type;
	public int value;
	public boolean local;
	public String name;
	
	public Relocation(int offset, String type, int value, boolean local, String name) {
		this.offset = offset;
		this.type = type;
		this.value = value;
		this.local = local;
		this.name = name;
	}
	
	
	@Override
	public String toString() {
		return String.format("Offset %08X\tValue %08X\t%c\tType %s", this.offset, this.value, this.local ? ' ' : 'E', this.type);
	}
	
	//get byte representation of relocation
	public byte[] getBytes() {
		/*
			u32 offset : 24;
			u32 type : 8;
			u32 value;
		*/
		int w0 = (relocTypeFromString(this.type, this.local) << 24) | this.offset;
		int w1 = this.value;
		
		byte[] b = {
			(byte) (w0 & 0xFF),
			(byte) ((w0 >>> 8) & 0xFF),
			(byte) ((w0 >>> 16) & 0xFF),
			(byte) ((w0 >>> 24) & 0xFF),
			(byte) (w1 & 0xFF),
			(byte) ((w1 >>> 8) & 0xFF),
			(byte) ((w1 >>> 16) & 0xFF),
			(byte) ((w1 >>> 24) & 0xFF)
		};
		return b;
	}
	
}