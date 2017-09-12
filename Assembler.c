#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main()	{
	FILE *mnemonic,*binary,*op,*reg,*sym;           //file pointers
	char opcode[100][2][40];                        //Array for storing OP Codes
	char regis[100][2][40];                         //Array for storing Registers
	char lbl[100][2][20];                           //Array for storing Labels
	char st[100],w[100];                            //temporary
	int line=1,i,j,regj=0,opj=0,labelj=0,stlen=0,q=0; //stlen for the instruction length count
                                                    //regj,labelj,opj and row numbers


	printf("\n\t\t\t******FIRE EMERGENCY SYSTEM******\n\n");
	/*
     *  Placing file pointers to the respective files
     */
	mnemonic = fopen("code.txt","r");
	op = fopen("Instructions.txt","r");
	reg = fopen("Register.txt","r");
	binary = fopen("bin.txt","w+");
	sym= fopen("sym.txt","w+");


	/*
	 * loop for finding colon in the string to identify the label in the string
	 * and then writing in symbol table file(sym.txt) its corresponding line number.
	 */
	fgets(st,100,mnemonic);
	do	{
		int len;
		len=strlen(st);
		for(i=0;i<len;i++)
			if(st[i]==':')	{
				st[i]='\0';
				fprintf(sym,"%s %d\n",st,line+1);
				strcpy(lbl[labelj][0],st);                    //Copy the string from st to label array
				itoa(line+1,lbl[labelj][1],10);
				labelj++;
			}
    	line++;
    	fgets(st,100,mnemonic);
	}while( !feof(mnemonic) );
	fseek(mnemonic,0,SEEK_SET);                 //Pass the file pointer back to the start of the file
                                                //so that we can use it again

    printf("Reading from Instructions.txt\n");
	i=0,j=0;
	do	{										//reading opcodes and their binary representation and writing them in array
		fscanf(op,"%s",st);
		if(i==0)	{
			strcpy(opcode[j][i],st);
			i++;
		}
		else	{
			strcpy(opcode[j][i],st);
			j++;
			i--;
		}
	}while( !feof(op) );
	opj=j;                                      //Now opj is the count of total rows i.e total OP Codes
	fclose(op);



	/*
	 * reading registers and their binary
	 * representation and writing them in array
     */

	printf("Reading from Register.txt\n");
	i=0,j=0;
	do	{
		fscanf(reg,"%s",st);
		if(i==0)	{
			strcpy(regis[j][i],st);
			i++;
		}
		else	{
			strcpy(regis[j][i],st);
			j++;
			i--;
		}
	}while( !feof(reg) );
	regj=j;
	fclose(reg);



	/*
	 * writing binary representation of assembly code.
	 */
	 printf("Generating Binary.txt\n\n");
	do	{
		fscanf(mnemonic,"%s",st);
		char temp1[15];
		int val,temp,len,fl=0;
		len=strlen(st);
		if(st[len-1]==':')	{
			fprintf(binary,"0000000000000000\n");					//skip if label instruction.
			continue;
		}

        //comparing string from file and opcode array and
        //then writing binary form in file
		for(j=0;j<opj;j++){
			if(strcmp(st,opcode[j][0])==0)	{
				fprintf(binary,"%s",opcode[j][1]);
				fl=1;
				stlen+=strlen(opcode[j][1]);        //to check the size of instruction
				if(q==1){
					printf("Error: put a ';' after instruction -> %s\n",w);
					return 0;
				}
				q=1;
				strcpy(w,st);
				break;
			}
		}

		if(q==0){
			printf("Error: Invalid Instruction -> %s\n",st);
			return 0;
		}

		//Comparing string from st to register array and then printing them in "Binary.txt"
		if(fl==0)	{
			for(j=0;j<regj;j++)	{
				if(strcmp(st,regis[j][0])==0)	{
					fprintf(binary,"%s",regis[j][1]);
					fl=1;
					stlen+=5;
					break;
				}
			}
		}

        //Comparing string from label array and converting it to binary
        //and then printing them in "Binary.txt"
        //only when jump instructions are used
		if(fl==0)	{
				for(j=0;j<labelj;j++)	{
					if(strcmp(st,lbl[j][0])==0)	{
						val=atoi(lbl[j][1]);
						strcpy(temp1,"00000");      //As we need exactly 5bit
						j=4;
						while(val>0)	{           //Converting to binary
							temp=val%2;
							val/=2;
							temp1[j]=temp+'0';
							j--;
						}
						fprintf(binary,"%s",temp1);
						fl=1;
						stlen+=5;
						break;
					}
                }
        }

		if(fl==0 && strcmp(st,";")!=0)	{		//when immediate values are encountered in assembly code
                                                //for 5 bit immediate values(used in LOAD,ADD,SUB,CMP instructions)
				val=atoi(st);
				strcpy(temp1,"00000");          //As we need exactly 5 bit
				j=4;
				while(val>0)	{               //Converting to binary
					temp=val%2;
					val/=2;
					temp1[j]=temp+'0';
					j--;
				}
				fprintf(binary,"%s",temp1);
				stlen+=5;
		}

		//checking for end of instruction
		if(strcmp(st,";")==0){
			fprintf(binary,"\n");
			if(stlen==16 && q == 1){                  //Check for the instruction length is 16bit or not
				stlen=0;
			}
			else{
				printf("Error: Improper Instruction Format -> %s\n",w);
				return 0;
			}
			q=0;				//Semicolon found and assign q back to 0
		}

	}while( !feof(mnemonic) );
    if(q ==1){
        printf("Error: put a ';' after instruction -> %s\n",w);
    }
	fclose(mnemonic);
	fclose(binary);
	return 0;
}
