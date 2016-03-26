#include<stdio.h>
#include<string.h>
#include<stdlib.h>

unsigned int hex[300]={0},in[4]={0},mem[300]={0},reg[32]={0};
int pc,pctmp;

void Instructionwrite(int count, FILE* f){
    int i=0;
    fprintf(f, "cycle %d\n", count);

    for(i=0;i<32;i++){
        fprintf(f, "$%.2d: 0x%.8X\n", i, reg[i]);
    }

    fprintf(f, "PC: 0x%.8X\n\n\n", pc);
}

int main(int argc, char* argv[])
{
    FILE *fp1;
	FILE *fp2;
	FILE *fp3;
	FILE *fp4;

	int num_i=1,num_d=1,t=0,/*pctmp=0,pc=0,*/count=0,p=0;
	int i=0,temp=0,tt;

	int opcode=0,rs=0,rt=0,rd=0,shamt=0,funct=0,address=0;

	short immediate,t1;
    //unsigned int hex[300]={0},in[4]={0},mem[300]={0},reg[32]={0};
    unsigned int temp_u=0;

    fp1=fopen("C:/Users/jimmy/Desktop/iimage.bin", "rb");
    while(!feof(fp1))//iimage
    {
        if(num_i==0&&t!=0)
        {
            break;
        }
        if(t==0){
            fread(&in[0], sizeof(unsigned char), 1, fp1);
            fread(&in[1], sizeof(unsigned char), 1, fp1);
            fread(&in[2], sizeof(unsigned char), 1, fp1);
            fread(&in[3], sizeof(unsigned char), 1, fp1);
            pc=in[0]*16777216+in[1]*65536+in[2]*256+in[3];
            pctmp=pc;

        }else if(t==1){
            fread(&in[0], sizeof(unsigned char), 1, fp1);
            fread(&in[1], sizeof(unsigned char), 1, fp1);
            fread(&in[2], sizeof(unsigned char), 1, fp1);
            fread(&in[3], sizeof(unsigned char), 1, fp1);
            num_i=in[0]*16777216+in[1]*65536+in[2]*256+in[3];
        }else{
            fread(&in[0], sizeof(unsigned char), 1, fp1);
            fread(&in[1], sizeof(unsigned char), 1, fp1);
            fread(&in[2], sizeof(unsigned char), 1, fp1);
            fread(&in[3], sizeof(unsigned char), 1, fp1);
            hex[t-2]=in[0]*16777216+in[1]*65536+in[2]*256+in[3];
            num_i--;
        }
        t++;
    }

    fclose(fp1);
    fp2=fopen("C:/Users/jimmy/Desktop/dimage.bin", "rb");

    t=0;

    while(!feof(fp2))//dimage
    {
        if(num_d==0&&t!=0){
            break;
        }
        if(t==0){
            fread(&in[0], sizeof(unsigned char), 1, fp1);
            fread(&in[1], sizeof(unsigned char), 1, fp1);
            fread(&in[2], sizeof(unsigned char), 1, fp1);
            fread(&in[3], sizeof(unsigned char), 1, fp1);
            reg[29]=in[0]*16777216+in[1]*65536+in[2]*256+in[3];

        }else if(t==1){
            fread(&in[0], sizeof(unsigned char), 1, fp1);
            fread(&in[1], sizeof(unsigned char), 1, fp1);
            fread(&in[2], sizeof(unsigned char), 1, fp1);
            fread(&in[3], sizeof(unsigned char), 1, fp1);
            num_d=in[0]*16777216+in[1]*65536+in[2]*256+in[3];

        }else{
            fread(&in[0], sizeof(unsigned char), 1, fp1);
            fread(&in[1], sizeof(unsigned char), 1, fp1);
            fread(&in[2], sizeof(unsigned char), 1, fp1);
            fread(&in[3], sizeof(unsigned char), 1, fp1);
            mem[t-2]=in[0]*16777216+in[1]*65536+in[2]*256+in[3];
            num_d--;
        }
        t++;
    }


    fclose(fp2);

    fp3=fopen("C:/Users/jimmy/Desktop/snapshot.rpt", "w");
    fp4=fopen("C:/Users/jimmy/Desktop/error_dump.rpt", "w");

    while(count<500001){

            Instructionwrite(count,fp3);
            count++;

            int tmpp;

            funct=hex[p]%64;
            immediate=hex[p]%65536;
            address=hex[p]%67108864;

            tmpp=(hex[p]/64);
            shamt=tmpp%32;

            tmpp=(hex[p]/2048);
            rd=tmpp%32;

            tmpp=(hex[p]/65536);
            rt=tmpp%32;

            tmpp=(hex[p]/2097152);
            rs=tmpp%32;

            tmpp=(hex[p]/67108864);
            opcode=tmpp%64;

            //Instrution begin
            if(opcode==43){    //sw
                temp=reg[rs]+immediate;

                if(reg[rs]>0&&immediate>0&&temp<0 || reg[rs]<0&&immediate<0&&temp>0){
                        fprintf(fp4 , "In cycle %d: Number Overflow\n", count);
                }
                if(temp<0){
                    fprintf(fp4 , "In cycle %d: Address Overflow\n", count);
                    temp=-temp;
                    if(temp%4!=0){
                        fprintf(fp4 , "In cycle %d: Misalignment Error\n", count);
                    }
                    break;
                }else if(temp>1023){
                    fprintf(fp4 , "In cycle %d: Address Overflow\n", count);

                    if(temp%4!=0){
                        fprintf(fp4 , "In cycle %d: Misalignment Error\n", count);
                    }
                    break;
                }else if(temp%4!=0){
                    fprintf(fp4 , "In cycle %d: Misalignment Error\n", count);
                    break;
                }

                temp=temp/4;
                mem[temp]=reg[rt];

                p++;
            }else if(opcode==3){    //jal
                reg[31]=pc+4;
                //int a=((pc+4)%268435456);
                pc=pc+4-((pc+4)%268435456);     // or
                pc=pc+4*address;
                p=(pc-pctmp)/4;
                pc=pc-4;
            }else if(opcode==4){     //beq
                temp=pc+4+immediate*4;
                if(reg[rs]==reg[rt]){
                    if((pc+4)>0&&immediate>0&&temp<0 || (pc+4)<0&&immediate<0&&temp>0){
                        fprintf(fp4 , "In cycle %d: Number Overflow\n", count);
                    }
                    pc=temp;
                    p=(pc-pctmp)/4;
                    pc=pc-4;
                }else  p++;

            }else if(opcode==5){     //bne
                temp=pc+4+immediate*4;
                if(reg[rs]!=reg[rt]){
                    if((pc+4)>0&&immediate>0&&temp<0 || (pc+4)<0&&immediate<0&&temp>0){
                        fprintf(fp4 , "In cycle %d: Number Overflow\n", count);
                    }
                    pc=temp;
                    p=(pc-pctmp)/4;
                    pc=pc-4;
                }else{
                    p++;
                }
            }else if(opcode==8){  //addi
                temp=reg[rs]+immediate;
                if(reg[rs]>0&&immediate>0&&temp<0 || reg[rs]<0&&immediate<0&&temp>0){
                    fprintf(fp4 , "In cycle %d: Number Overflow\n", count);
                }
                if(rt!=0){
                    reg[rt]=temp;
                }else{
                    fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                }
                p++;
            }else if(opcode==9){    //addiu
                temp_u=reg[rs]+immediate;
                if(rt!=0){
                    reg[rt]=temp_u;

                }else{
                    fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                }
                p++;
            }else if(opcode==2){     //j
                //int a=((pc+4)%268435456);
                pc=pc+4-((pc+4)%268435456);     // or
                pc=pc+4*address;
                p=(pc-pctmp)/4;
                pc=pc-4;
            }else if(opcode==32){    //lb
                temp=reg[rs]+immediate;

                int tmp = temp/4;     ////this
                int me;
                if(temp%4==0){
                    me=mem[tmp]/16777216;
                }else if(temp%4==1){
                    me=(mem[tmp]/65536)%256;
                }else if(temp%4==2){
                    me=(mem[tmp]/256)%256;
                }else if(temp%4==3){
                    me=mem[tmp]%256;
                }

                if(rt==0){
                    fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                }
                if(reg[rs]>0&&immediate>0&&temp<0 || reg[rs]<0&&immediate<0&&temp>0){
                    fprintf(fp4 , "In cycle %d: Number Overflow\n", count);
                }
                if(temp<0){
                    fprintf(fp4 , "In cycle %d: Address Overflow\n", count);
                    break;
                }else if(temp>1023){
                    fprintf(fp4 , "In cycle %d: Address Overflow\n", count);
                    break;
                }

                if(rt!=0) reg[rt]=(char)me;
                p++;
            }else if(opcode==33){    //lh
                temp=reg[rs]+immediate;

                int tmp = temp/4;
                int me;
                if(temp%4==0){
                    me=mem[tmp]/65536;
                }else if(temp%4==2){
                    me=mem[tmp]%65536;
                }

                if(rt==0){
                    fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                }
                if(reg[rs]>0&&immediate>0&&temp<0 || reg[rs]<0&&immediate<0&&temp>0){
                        fprintf(fp4 , "In cycle %d: Number Overflow\n", count);
                }
                if(temp<0){
                    fprintf(fp4 , "In cycle %d: Address Overflow\n", count);
                    temp=-temp;

                    if(temp%2!=0){
                        fprintf(fp4 , "In cycle %d: Misalignment Error\n", count);
                    }
                    break;
                }else if(temp>1023){
                    fprintf(fp4 , "In cycle %d: Address Overflow\n", count);

                    if(temp%4==1||temp%4==3){
                        fprintf(fp4 , "In cycle %d: Misalignment Error\n", count);
                    }
                    break;
                }else if(temp%2!=0){
                    fprintf(fp4 , "In cycle %d: Misalignment Error\n", count);
                    break;
                }

                if(rt!=0) reg[rt]=(short)me;
                p++;
            }else if(opcode==10){    //slti
                if(rt!=0){
                    reg[rt]=(reg[rs]<immediate);
                }else{
                    fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
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
                if(temp<0){
                    fprintf(fp4 , "In cycle %d: Address Overflow\n", count);

                    temp=-temp;

                    if(temp%4!=0){
                        fprintf(fp4 , "In cycle %d: Misalignment Error\n", count);
                    }
                    break;

                }else if(temp>1023){
                    fprintf(fp4 , "In cycle %d: Address Overflow\n", count);

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
                }
                p++;
            }else if(opcode==36){    //lbu
                temp=reg[rs]+immediate;

                int tmp = temp/4;     ////this
                int me;
                if(temp%4==0){
                    me=mem[tmp]/16777216;
                }else if(temp%4==1){
                    me=(mem[tmp]/65536)%256;
                }else if(temp%4==2){
                    me=(mem[tmp]/256)%256;
                }else if(temp%4==3){
                    me=mem[tmp]%256;
                }

                if(rt==0){
                    fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                }
                if(reg[rs]>0&&immediate>0&&temp<0 || reg[rs]<0&&immediate<0&&temp>0){
                    fprintf(fp4 , "In cycle %d: Number Overflow\n", count);
                }
                if(temp<0){
                    fprintf(fp4 , "In cycle %d: Address Overflow\n", count);
                    break;
                }else if(temp>1023){
                    fprintf(fp4 , "In cycle %d: Address Overflow\n", count);
                    break;
                }

                if(rt!=0) reg[rt]=me;

                p++;
            }else if(opcode==37){    //lhu
                temp=reg[rs]+immediate;

                int tmp = temp/4;
                int me;

                if(temp%4==0){              //////this
                    me=mem[tmp]/65536;
                }else if(temp%4==2){
                    me=mem[tmp]%65536;
                }

                if(rt==0){
                    fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                }
                if(reg[rs]>0&&immediate>0&&temp<0 || reg[rs]<0&&immediate<0&&temp>0){
                    fprintf(fp4 , "In cycle %d: Number Overflow\n", count);
                }
                if(temp<0){
                    fprintf(fp4 , "In cycle %d: Address Overflow\n", count);
                    temp=-temp;
                    if(temp%2!=0){
                        fprintf(fp4 , "In cycle %d: Misalignment Error\n", count);
                    }
                    break;
                }else if(temp>1023){
                    fprintf(fp4 , "In cycle %d: Address Overflow\n", count);

                    if(temp%2!=0){
                        fprintf(fp4 , "In cycle %d: Misalignment Error\n", count);
                    }
                    break;

                }
                if(temp%2!=0){
                    fprintf(fp4 , "In cycle %d: Misalignment Error\n", count);
                    break;
                }

                if(rt!=0) reg[rt]=me;
                p++;
            }else if(opcode==36){    //lbu
                temp=reg[rs]+immediate;

                int tmp = temp/4;     ////this
                int me;
                if(temp%4==0){
                    me=mem[tmp]/16777216;
                }else if(temp%4==1){
                    me=(mem[tmp]/65536)%256;
                }else if(temp%4==2){
                    me=(mem[tmp]/256)%256;
                }else if(temp%4==3){
                    me=mem[tmp]%256;
                }

                if(rt==0){
                    fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                }
                if(reg[rs]>0&&immediate>0&&temp<0 || reg[rs]<0&&immediate<0&&temp>0){
                    fprintf(fp4 , "In cycle %d: Number Overflow\n", count);
                }
                if(temp>1023){
                    fprintf(fp4 , "In cycle %d: Address Overflow\n", count);
                    break;
                }else if(temp<0){
                    fprintf(fp4 , "In cycle %d: Address Overflow\n", count);
                    break;
                }

                if(rt!=0) reg[rt]=me;

                p++;
            }else if(opcode==41){    //sh
                temp=reg[rs]+immediate;

                if(reg[rs]>0&&immediate>0&&temp<0 || reg[rs]<0&&immediate<0&&temp>0){
                        fprintf(fp4 , "In cycle %d: Number Overflow\n", count);
                }
                if(temp>1023){
                    fprintf(fp4 , "In cycle %d: Address Overflow\n", count);
                    if(temp%2!=0){
                        fprintf(fp4 , "In cycle %d: Misalignment Error\n", count);
                    }
                    break;
                }else if(temp<0){
                    temp=-temp;
                    if(temp%2!=0){
                        fprintf(fp4 , "In cycle %d: Misalignment Error\n", count);
                    }
                    break;
                }else if(temp%2!=0){
                    fprintf(fp4 , "In cycle %d: Misalignment Error\n", count);
                    break;
                }

                int me=temp/4;   ////this
                int regtmp,memtmp;
                if(temp%2==0){
                    regtmp=reg[rt]&0x0000FFFF;
                }
                if(temp%4==0){
                    memtmp=mem[me]&0x0000FFFF;
                    mem[me]=regtmp*65536+memtmp;
                }else if(temp%4==2){
                    memtmp=mem[me]&0xFFFF0000;
                    mem[me]=memtmp+regtmp;
                }
                p++;
            }else if(opcode==14){    //nori
                if(rt!=0){
                    reg[rt]=~(reg[rs]|(unsigned short)immediate);
                }else{
                    fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                }
                p++;
            }else if(opcode==40){    //sb
                int tmm;
                if(reg[rs]>0&&immediate>0&&reg[rs]+immediate<0 || reg[rs]<0&&immediate<0&&reg[rs]+immediate>0){
                        fprintf(fp4 , "In cycle %d: Number Overflow\n", count);
                }
                if(reg[rs]+immediate<0){
                    fprintf(fp4 , "In cycle %d: Address Overflow\n", count);
                    break;
                }else if(reg[rs]+immediate>1023){
                    fprintf(fp4 , "In cycle %d: Address Overflow\n", count);
                    break;
                }

                temp=reg[rs]+immediate;

                int me=temp/4;    ////this
                int regtmp=reg[rt]&0x000000FF;
                int memtmp,memtmp2;
                if(regtmp%4==0){
                    memtmp=mem[me]&0x00FFFFFF;
                    mem[me]=regtmp*16777216+memtmp;
                }else if(regtmp%4==1){
                    memtmp=mem[me]&0x0000FFFF;
                    memtmp2=mem[me]&0xFF000000;
                    mem[me]=memtmp2+regtmp*65536+memtmp;
                }else if(regtmp%4==2){
                    memtmp=mem[me]&0x000000FF;
                    memtmp2=mem[me]&0xFFFF0000;
                    mem[me]=memtmp2+regtmp*256+memtmp;
                }else if(regtmp%4==3){
                    memtmp=mem[me]&0xFFFFFF00;
                    mem[me]=memtmp+regtmp;
                }
                p++;
            }else if(opcode==63){    //halt
                break;
            }else if(opcode==12){    //andi
                if(rt==0){
                    fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                }else{
                    reg[rt]=reg[rs]&(unsigned short)immediate;
                }
                p++;
            }else if(opcode==7){     //bgtz
                if(reg[rs]>0){
                    pc=pc+4+4*immediate;
                    p=(pc-pctmp)/4;
                    pc=pc-4;
                }else{
                    p++;
                }
            }else if(opcode==13){    //ori
                if(rt==0){
                    fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                }else{
                    reg[rt]=reg[rs]|(unsigned short)immediate;
                }
                p++;
            }else if(opcode==0){
                if(funct==32){   //add
                    temp=reg[rt]+reg[rs];

                    if(reg[rt]>0&&reg[rs]>0&&temp<0 || reg[rt]<0&&reg[rs]<0&&temp>0){
                        fprintf(fp4 , "In cycle %d: Number Overflow\n", count);
                    }
                    if(rd!=0){
                        reg[rd]=temp;
                    }else{
                        fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                    }
                    p++;
                }else if(funct==8){   // jr    ////
                    pc=reg[rs];
                    p=(pc-pctmp)/4;
                    pc-=4;
                }else if(funct==39){   //nor

                    if(rd!=0){
                        reg[rd]=~(reg[rs]|reg[rt]);
                    }
                    else{
                        fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                    }
                    p++;
                }else if(funct==0){    //sll   ///
                    if(rd==0&&rt==0&&shamt==0){
                        pc=pc+4;
                        p++;
                        continue;
                    }else if(rd==0){
                        fprintf(fp4, "In cycle %d: Write $0 Error\n", count);
                    }
                    if(rd!=0)
                        reg[rd]=reg[rt]<<shamt;
                    p++;
                }else if(funct==3){   //sra   /////////
                    if(rd==0){
                        fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                    }
                    else{
                        reg[rd]=reg[rt]>>shamt;
                    }
                    p++;
                }else if(funct==2){  //srl
                    if(rd==0){
                        fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                    }
                    else{
                        reg[rd]=(unsigned int)reg[rt]>>shamt;
                    }
                    p++;
                }else if(funct==34){  //sub
                    temp = -reg[rt];
                    if(reg[rs]>0&&temp>0&&reg[rs]+temp<0 || reg[rs]<0&&temp<0&&reg[rs]+temp>0){
                        fprintf(fp4 , "In cycle %d: Number Overflow\n", count);
                    }
                    if(rd==0){
                        fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                    }
                    else{
                        reg[rd]=reg[rs]+temp;
                    }
                    p++;
                }else if(funct==37){   //or
                    if(rd!=0){
                        reg[rd]=reg[rs]|reg[rt];
                    }
                    else{
                        fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                    }
                    p++;
                }else if(funct==38){   //xor
                    if(rd!=0){
                        reg[rd]=reg[rs]^reg[rt];
                    }
                    else{
                        fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                    }
                    p++;
                }else if(funct==40){   //nand
                    if(rd!=0){
                        reg[rd]=~(reg[rs]&reg[rt]);
                    }
                    if(rd!=0){
                        fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                    }
                    p++;
                }else if(funct==42){   //slt
                    if(rd!=0){
                        reg[rd]=(reg[rs]<reg[rt]);
                    }
                    else{
                        fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                    }
                    p++;
                }else if(funct==33){ //addu
                    temp_u=reg[rt]+reg[rs];
                    if(rd!=0){
                        reg[rd]=temp_u;
                    }else {
                        fprintf( fp4 , "In cycle %d: Write $0 Error\n", count);
                    }
                    p++;
                }
            }
            pc=pc+4;
    }

    fclose(fp3);
    fclose(fp4);

    return 0;
}

