#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void){
	FILE *fp;
	FILE *fp2;
	FILE *fp3;
	FILE *fp4;
	int num_i=0,num_d=0,t=0,pctmp=0,pc=0,count=0,p=0,nut=0;
	int i=0,temp=0;
	unsigned int temp_u=0;
	int reg[32]={0};
	unsigned int hex[600]={0},in[4]={0},mem[300]={0};
	int opcode=0,rs=0,rt=0,rd=0,shamt=0,funct=0,address=0;
	short immediate;

	fp=fopen("iimage.bin", "rb");

	while(!feof(fp)){ //iimage
        if(num_i==0&&t>1){
			break;
		}else if(t==0){
			fread(&in[0], sizeof(unsigned char), 1, fp);
			fread(&in[1], sizeof(unsigned char), 1, fp);
			fread(&in[2], sizeof(unsigned char), 1, fp);
			fread(&in[3], sizeof(unsigned char), 1, fp);
			pc=in[0]*16777216+in[1]*65536+in[2]*256+in[3];
			pctmp=pc;

		}else if(t==1){
			fread(&in[0], sizeof(unsigned char), 1, fp);
			fread(&in[1], sizeof(unsigned char), 1, fp);
			fread(&in[2], sizeof(unsigned char), 1, fp);
			fread(&in[3], sizeof(unsigned char), 1, fp);
			num_i=in[0]*16777216+in[1]*65536+in[2]*256+in[3];

		}else{
			fread(&in[0], sizeof(unsigned char), 1, fp);
			fread(&in[1], sizeof(unsigned char), 1, fp);
			fread(&in[2], sizeof(unsigned char), 1, fp);
			fread(&in[3], sizeof(unsigned char), 1, fp);
			hex[t-2]=in[0]*16777216+in[1]*65536+in[2]*256+in[3];
			num_i--;
		}
		t++;
    }
    fclose(fp);
	fp2=fopen("dimage.bin", "rb");
	t=0;
	while(!feof(fp2)){ //dimage
        if(num_d==0&&t>1){
			break;
		}else if(t==0){
			fread(&in[0], sizeof(unsigned char), 1, fp);
			fread(&in[1], sizeof(unsigned char), 1, fp);
			fread(&in[2], sizeof(unsigned char), 1, fp);
			fread(&in[3], sizeof(unsigned char), 1, fp);
			reg[29]=in[0]*16777216+in[1]*65536+in[2]*256+in[3];

		}else if(t==1){
			fread(&in[0], sizeof(unsigned char), 1, fp);
			fread(&in[1], sizeof(unsigned char), 1, fp);
			fread(&in[2], sizeof(unsigned char), 1, fp);
			fread(&in[3], sizeof(unsigned char), 1, fp);
			num_d=in[0]*16777216+in[1]*65536+in[2]*256+in[3];

		}else{
			fread(&in[0], sizeof(unsigned char), 1, fp);
			fread(&in[1], sizeof(unsigned char), 1, fp);
			fread(&in[2], sizeof(unsigned char), 1, fp);
			fread(&in[3], sizeof(unsigned char), 1, fp);
			mem[t-2]=in[0]*16777216+in[1]*65536+in[2]*256+in[3];
			num_d--;
		}
		t++;
    }
    fclose(fp2);
    fp3=fopen("snapshot.rpt", "w");  //write answer
    fp4=fopen("error_dump.rpt", "w"); //write error

    while(1){
        if(p<0){
            fprintf(fp3, "cycle %d\n", count);
            for(i=0;i<32;i++){
                fprintf(fp3, "$%.2d: 0x%.8X\n", i, reg[i]);
            }
            fprintf(fp3, "PC: 0x%.8X\n\n\n", pc);
            count++;
            if(count>500000)break;

            p++;
            pc=pc+4;
            continue;
        }

        nut++;
        fprintf(fp3, "cycle %d\n", count);
        for(i=0;i<32;i++){
            fprintf(fp3, "$%.2d: 0x%.8X\n", i, reg[i]);
        }
        fprintf(fp3, "PC: 0x%.8X\n\n\n", pc);
        count++;

        if(count>500000)break;

        opcode=(hex[p]/6710886)%64;
        rs=(hex[p]/2097152)%32;
        rt=(hex[p]/65536)%32;
        rd=(hex[p]/2048)%32;
        shamt=(hex[p]/64)%32;
        funct=hex[p]%64;
        immediate=hex[p]%65536;
        address=hex[p]%67108864;

        if(opcode==0){
            if(funct==32){   //add
                temp=reg[rt]+reg[rs];

                if(reg[rt]>0&&reg[rs]>0&&temp<0 || reg[rt]<0&&reg[rs]<0&&temp>0){
                    fprintf(fp4 , "In cycle %d: Number Overflow\n", count);
                }
                if(rd==0){
                    fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                }else{
                    reg[rd]=temp;
                }
                p++;
            }else if(funct==33){ //addu
                temp_u=reg[rt]+reg[rs];
                if(rd==0){
                    fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                }else {
                    reg[rd]=temp_u;
                }
                p++;
            }else if(funct==34){  //sub
                temp = reg[rs]-reg[rt];
                if(reg[rt]>0&&reg[rs]>0&&temp<0 || reg[rt]<0&&reg[rs]<0&&temp>0){
                    fprintf(fp4 , "In cycle %d: Number Overflow\n", count);
                }
                if(rd==0){
                    fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                }
                else{
                    reg[rd]=temp;
                }
                p++;
            }else if(funct==36){  //and
                if(rd==0){
                    fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                }
                else{
                    reg[rd]=reg[rs]&reg[rt];
                }
                p++;
            }else if(funct==37){   //or
                if(rd==0){
                    fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                }
                else{
                    reg[rd]=reg[rs]|reg[rt];
                }
                p++;
            }else if(funct==38){   //xor
                if(rd==0){
                    fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                }
                else{
                    reg[rd]=reg[rs]^reg[rt];
                }
                p++;
            }else if(funct==39){   //nor
                if(rd==0){
                    fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                }
                else{
                    reg[rd]=~(reg[rs]|reg[t]);
                }
                p++;
            }else if(funct==40){   //nand
                if(rd==0){
                    fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                }
                else{
                    reg[rd]=~(reg[rs]&reg[rt]);
                }
                p++;
            }else if(funct==42){   //slt
                if(rd==0){
                    fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                }
                else{
                    reg[rd]=(reg[rs]<reg[rt]);
                }
                p++;
            }else if(funct==0){    //sll
                if(rd==0&&rt==0&&shamt==0){
                    p++;
                    pc+=4;
                    continue;
                }else if(rd==0){
                    fprintf(fp4, "In cycle %d: Write $0 Error\n", count);
                }
                if(rd!=0)
                    reg[rd]=reg[rt]<<shamt;
                p++;
            }else if(funct==2){  //srl
                if(rd==0){
                    fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                }
                else{
                    reg[rd]=(unsigned int)reg[rt]>>shamt;
                }
                p++;
            }else if(funct==3){   //sra   /////////
                if(rd==0){
                    fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                }
                else{
                    reg[rd]=reg[rt]>>shamt;

                }
                p++;
            }else if(funct==8){   // jr
                pc=reg[rs];
                p=(pc-pctmp)/4;
                pc-=4;
            }
        }else if(opcode==8){  //addi
            temp=reg[rs]+immediate;
            if(reg[rs]>0&&immediate>0&&temp<0 || reg[rs]<0&&immediate<0&&temp>0){
                fprintf(fp4 , "In cycle %d: Number Overflow\n", count);
            }
            if(rt==0){
                fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
            }else{
                reg[rt]=temp;
            }
            p++;
        }else if(opcode==9){    //addiu
            temp_u=reg[rs]+immediate;
            if(rt==0){
                fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
            }else{
                reg[rt]=temp_u;
            }
            p++;
        }else if(opcode==35){    //lw
            temp=reg[rs]+immediate;

            if(rt==0){
                fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
            }
            if(reg[rs]>0&&immediate>0&&temp<0 || reg[rs]<0&&immediate<0&&temp>0){
                    fprintf(fp4 , "In cycle %d: Number Overflow\n", count);
            }
            if(temp>1023||temp<0){
                fprintf(fp4 , "In cycle %d: Address Overflow\n", count);
                if(temp<0) temp=-temp;

                if(temp%4!=0){
                    fprintf(fp4 , "In cycle %d: Misalignment Error\n", count);
                }
                break;
            }else if(temp%4!=0){
                fprintf(fp4 , "In cycle %d: Misalignment Error\n", count);
                break;
            }
            if(rt!=0){
                temp=temp/4;
                reg[rt]=mem[temp];
                p++;
            }
        }else if(opcode==33){

        }else if(opcode==33){

        }else if(opcode==33){

        }else if(opcode==33){

        }else if(opcode==33){

        }else if(opcode==33){

        }else if(opcode==33){

        }else if(opcode==33){

        }else if(opcode==33){

        }
    }
}
