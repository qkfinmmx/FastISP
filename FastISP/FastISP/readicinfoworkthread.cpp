#include "readicinfoworkthread.h"

enum {
	Stop,
	Start,
	SendInfoCMD,
	PIDCMDInfo,
	UIDCMDInf,
	CLRCMD,
	WriteMemory
};

enum{
	RICInfo,
	CLRIC,
	WMIC
};

ReadICINFOWorkThread::ReadICINFOWorkThread(QObject *parent)
	: QThread(parent)
{
	
}

ReadICINFOWorkThread::~ReadICINFOWorkThread()
{

}

void ReadICINFOWorkThread::Init( DWORD dwportnum,DWORD dwbaudrate,CnComm* pCncom ,int type)
{
	m_portnum = dwportnum;
	m_baudrate = dwbaudrate;
	m_cncom = pCncom;
	m_type = type;
}

void ReadICINFOWorkThread::InitWM( DWORD dwportnum,DWORD dwbaudrate,CnComm* pCncom,QStringList str,int type )
{
	m_portnum = dwportnum;
	m_baudrate = dwbaudrate;
	m_cncom = pCncom;
	m_type = type;
	m_strwrite = str;
}

void ReadICINFOWorkThread::run()
{
	switch(m_type)
	{
	case RICInfo:
		{
			m_flag = Start;
			int t_count = 0;
			QString tstr_count;
			QString tstr_portnum;
			QString tstr_baudrate;
			tstr_portnum = tstr_portnum.sprintf("%d",m_portnum);
			tstr_baudrate = tstr_baudrate.sprintf("%d",m_baudrate);
			while (m_flag != Stop){
				if(t_count > 200){
					m_flag = Stop;
					emit threadoutput(QStringLiteral("等待超时...300 ！"),0);
				}
				switch(m_flag)
				{
				case Start:
					{
						unsigned char t_cmd[1];
						unsigned char t_revACK[1];
						t_cmd[0] = 0x7f;
						m_cncom->Write(t_cmd,1);
						t_count++;
						tstr_count = tstr_count.sprintf("%d",t_count);
						if (m_cncom->Read(t_revACK,1)>0){
							if (t_revACK[0]==0x79){
								m_flag = SendInfoCMD;
								emit threadoutput(QStringLiteral("开始连接...尝试")+tstr_count+QStringLiteral("次.接收到ACK信号 ！\n"),0);
								emit threadoutput(QStringLiteral("在串口COM")+tstr_portnum+QStringLiteral("连接成功@")+tstr_baudrate+QStringLiteral("bps !"),0);
							}
							else{
								emit threadoutput(QStringLiteral("开始连接...尝试")+tstr_count+QStringLiteral("次.接收到NACK信号...连接失败正在重试... \n"),1);
							}
						}
						else{
							emit threadoutput(QStringLiteral("开始连接...尝试")+tstr_count+QStringLiteral("次.接收到NACK信号...连接失败正在重试... \n"),1);
						}
						break;
					}
				case Stop:
					{
						break;
					}
				case SendInfoCMD:
					{
						unsigned char t_cmd[2];
						unsigned char t_revINFO[15];
						t_cmd[0] = 0x00;
						t_cmd[1] = 0xff;
						m_cncom->Write(t_cmd,2);
						if (m_cncom->Read(t_revINFO,15)>0){
							if (t_revINFO[0] == 0x79 && t_revINFO[14] == 0x79){
								m_flag = PIDCMDInfo;
								emit threadoutput(QStringLiteral("芯片内BootLoader版本号:")+TransBootloadVersion(t_revINFO[2]),0);
							}
						}
						break;
					}
				case PIDCMDInfo:
					{
						unsigned char t_cmd[2];
						unsigned char t_revINFO[5];
						t_cmd[0] = 0x02;
						t_cmd[1] = 0xfd;
						m_cncom->Write(t_cmd,2);
						if (m_cncom->Read(t_revINFO,5)>0){
							if (t_revINFO[0] == 0x79 && t_revINFO[4] == 0x79){
								m_flag = UIDCMDInf;
								switch(t_revINFO[3])
								{
								case 0x10:
									{
										emit threadoutput(QStringLiteral("芯片PID:0x0410"),0);
										break;
									}
								case 0x12:
									{
										emit threadoutput(QStringLiteral("芯片PID:0x0412"),0);
										break;
									}
								case 0x14:
									{
										emit threadoutput(QStringLiteral("芯片PID:0x0414"),0);
										break;
									}
								default:
									break;
								}
							}
						}
					}
				case UIDCMDInf:
					{
						unsigned char t_cmd1[2];
						unsigned char t_revINFO1[1];
						unsigned char t_cmd2[5];
						unsigned char t_revINFO2[1];
						unsigned char t_cmd3[2];
						unsigned char t_revINFO3[23];
						QString t_outinfo = "";
						t_cmd1[0] = 0x11;
						t_cmd1[1] = 0xee;
						t_cmd2[0] = 0x1f;
						t_cmd2[1] = 0xff;
						t_cmd2[2] = 0xf7;
						t_cmd2[3] = 0xe0;
						t_cmd2[4] = 0xf7;
						t_cmd3[0] = 0x15;
						t_cmd3[1] = 0xea;
						m_cncom->Write(t_cmd1,2);
						if (m_cncom->Read(t_revINFO1,1)>0){
							if (t_revINFO1[0] == 0x79){
								m_cncom->Write(t_cmd2,5);
								if (m_cncom->Read(t_revINFO2,1)>0){
									if (t_revINFO2[0] == 0x79){
										m_cncom->Write(t_cmd3,2);
										if (m_cncom->Read(t_revINFO3,23)>0){
											if (t_revINFO3[0] == 0x79){
												for (int i=5;i<21;i++){
													t_outinfo+=Transuchtoqstring(t_revINFO3[i]);
												}
												m_flag = Stop;
												emit threadoutput(QStringLiteral("芯片UID:")+t_outinfo,0);
											}
										}
									}
								}
							}
						}
						break;
					}
				default:
					break;
				}
			}
			break;
		}
	case CLRIC:
		{
			m_flag = Start;
			int t_count = 0;
			QString tstr_count;
			QString tstr_portnum;
			QString tstr_baudrate;
			tstr_portnum = tstr_portnum.sprintf("%d",m_portnum);
			tstr_baudrate = tstr_baudrate.sprintf("%d",m_baudrate);
			while (m_flag != Stop){
				if(t_count > 200){
					m_flag = Stop;
					emit threadoutput(QStringLiteral("等待超时...300 ！"),0);
				}
				switch(m_flag)
				{
				case Start:
					{
						unsigned char t_cmd[1];
						unsigned char t_revACK[1];
						t_cmd[0] = 0x7f;
						m_cncom->Write(t_cmd,1);
						t_count++;
						tstr_count = tstr_count.sprintf("%d",t_count);
						if (m_cncom->Read(t_revACK,1)>0){
							if (t_revACK[0]==0x79){
								m_flag = CLRCMD;
								emit threadoutput(QStringLiteral("开始连接...尝试")+tstr_count+QStringLiteral("次.接收到ACK信号 ！\n"),0);
								emit threadoutput(QStringLiteral("在串口COM")+tstr_portnum+QStringLiteral("连接成功@")+tstr_baudrate+QStringLiteral("bps !"),0);
							}
							else{
								emit threadoutput(QStringLiteral("开始连接...尝试")+tstr_count+QStringLiteral("次.接收到NACK信号...连接失败正在重试... \n"),1);
							}
						}
						else{
							emit threadoutput(QStringLiteral("开始连接...尝试")+tstr_count+QStringLiteral("次.接收到NACK信号...连接失败正在重试... \n"),1);
						}
						break;
					}
				case Stop:
					{
						break;
					}
				case CLRCMD:
					{
						unsigned char t_cmd[2];
						unsigned char t_revACK[1];
						t_cmd[0] = 0x43;
						t_cmd[1] = 0xbc;
						m_cncom->Write(t_cmd,2);
						if (m_cncom->Read(t_revACK,1)>0){
							if (t_revACK[0] == 0x79){
								unsigned char t_cmd2[2];
								unsigned char t_revACK2[1];
								t_cmd2[0] = 0xff;
								t_cmd2[1] = 0x00;
								m_cncom->Write(t_cmd2,2);
								if (m_cncom->Read(t_revACK2,1)>0){
									if (t_revACK2[0] == 0x79){
										m_flag = Stop;
										emit threadoutput(QStringLiteral("芯片全片擦除成功 !"),0);
									}
								}
							}
						}
						break;
					}
				default:
					break;
				}
			}
			break;
		}
	case WMIC:
		{
			m_flag = Start;
			int t_count = 0;
			QString tstr_count;
			QString tstr_portnum;
			QString tstr_baudrate;
			tstr_portnum = tstr_portnum.sprintf("%d",m_portnum);
			tstr_baudrate = tstr_baudrate.sprintf("%d",m_baudrate);
			while (m_flag != Stop){
				if(t_count > 200){
					m_flag = Stop;
					emit threadoutput(QStringLiteral("等待超时...300 ！"),0);
				}
					switch(m_flag)
					{
					case Start:
						{
							unsigned char t_cmd[1];
							unsigned char t_revACK[1];
							t_cmd[0] = 0x7f;
							m_cncom->Write(t_cmd,1);
							t_count++;
							tstr_count = tstr_count.sprintf("%d",t_count);
							if (m_cncom->Read(t_revACK,1)>0){
								if (t_revACK[0]==0x79){
									m_flag = CLRCMD;
									emit threadoutput(QStringLiteral("开始连接...尝试")+tstr_count+QStringLiteral("次.接收到ACK信号 ！\n"),0);
									emit threadoutput(QStringLiteral("在串口COM")+tstr_portnum+QStringLiteral("连接成功@")+tstr_baudrate+QStringLiteral("bps !"),0);
								}
								else{
									emit threadoutput(QStringLiteral("开始连接...尝试")+tstr_count+QStringLiteral("次.接收到NACK信号...连接失败正在重试... \n"),1);
								}
							}
							else{
								emit threadoutput(QStringLiteral("开始连接...尝试")+tstr_count+QStringLiteral("次.接收到NACK信号...连接失败正在重试... \n"),1);
							}
							break;
						}
					case Stop:
						{
							break;
						}
					case CLRCMD:
						{
							unsigned char t_cmd[2];
							unsigned char t_revACK[1];
							t_cmd[0] = 0x43;
							t_cmd[1] = 0xbc;
							m_cncom->Write(t_cmd,2);
							if (m_cncom->Read(t_revACK,1)>0){
								if (t_revACK[0] == 0x79){
									unsigned char t_cmd2[2];
									unsigned char t_revACK2[1];
									t_cmd2[0] = 0xff;
									t_cmd2[1] = 0x00;
									m_cncom->Write(t_cmd2,2);
									if (m_cncom->Read(t_revACK2,1)>0){
										if (t_revACK2[0] == 0x79){
											m_flag = WriteMemory;
											emit threadoutput(QStringLiteral("芯片全片擦除成功 !"),0);
										}
									}
								}
							}
							break;
						}
					case WriteMemory:
						{
							emit threadoutput(QStringLiteral("开始写入数据 !"),0);
							m_strwrite.pop_back();		//去除最后一项空字符串
							unsigned char t_cmdstart[2];
							unsigned char t_rcv[1];
							unsigned char t_cmdMSB[2];
							unsigned char t_cmdLSB[2];
							t_cmdstart[0] = 0x31;
							t_cmdstart[1] = 0xce;
							for (int i=0;i<m_strwrite.size();i++){
								if (m_strwrite.at(i).mid(7,2).compare("00") == 0){
									unsigned char* t_datacmd;
									int t_cmdlen = m_strwrite.at(i).mid(1,2).toInt(0,16);
									t_datacmd = new unsigned char[t_cmdlen];
									converttohexstr(m_strwrite.at(i).mid(9,t_cmdlen*2),t_datacmd);
									unsigned char t_lencmd[1];
									t_lencmd[0] = t_cmdlen-1;
									int t_cmdsumlen = t_cmdlen+2;
									unsigned char* t_cmdres;
									t_cmdres = new unsigned char[t_cmdsumlen];
									memset(t_cmdres,0,sizeof(unsigned char)*t_cmdsumlen);
									t_cmdres[0] = t_lencmd[0];
									for (int k=1;k<t_cmdsumlen-1;k++){
										t_cmdres[k] = t_datacmd[k-1];
									}
									t_cmdres[t_cmdsumlen-1] = t_cmdres[0];
									for (int j=1;j<t_cmdsumlen-1;j++){
										t_cmdres[t_cmdsumlen-1] ^= t_cmdres[j];
									}

									converttohexstr(m_strwrite.at(i).mid(3,4),t_cmdLSB);
									unsigned char t_addresscmd[5];
									t_addresscmd[0]=t_cmdMSB[0];
									t_addresscmd[1]=t_cmdMSB[1];
									t_addresscmd[2]=t_cmdLSB[0];
									t_addresscmd[3]=t_cmdLSB[1];
									t_addresscmd[4]=t_cmdMSB[0]^t_cmdMSB[1]^t_cmdLSB[0]^t_cmdLSB[1];

									unsigned char t_rcv1[1];
									unsigned char t_rcv2[1];
									unsigned char t_rcv3[1];
									m_cncom->Write(t_cmdstart,2);
									if (m_cncom->Read(t_rcv1,1)>0){
										if (t_rcv1[0] == 0x79){
											m_cncom->Write(t_addresscmd,5);
											if (m_cncom->Read(t_rcv2,1)>0){
												if (t_rcv2[0]==0x79){
													m_cncom->Write(t_cmdres,t_cmdsumlen);
													if (m_cncom->Read(t_rcv3,1)>0){
														if (t_rcv3[0] == 0x79){
															emit processshow(i*100/m_strwrite.size());
															continue;
														}
													}
												}
											}
										}
									}	
								}
								if (m_strwrite.at(i).mid(7,2).compare("01") == 0){
									emit threadoutput(QStringLiteral("写入数据完成 !"),0);
								}
								if (m_strwrite.at(i).mid(7,2).compare("02") == 0){

								}
								if (m_strwrite.at(i).mid(7,2).compare("03") == 0){

								}
								if (m_strwrite.at(i).mid(7,2).compare("04") == 0){
									converttohexstr(m_strwrite.at(i).mid(9,4),t_cmdMSB);
								}
								if (m_strwrite.at(i).mid(7,2).compare("05") == 0){

								}
							}
							emit processshow(100);
							m_flag = Stop;
							break;
						}
					default:
						break;
					}
				}
			}
		default:
			break;
	}
	
	m_cncom->Close();
	emit threadoutput(QStringLiteral("串口关闭 !"),0);
}

QString ReadICINFOWorkThread::TransBootloadVersion( unsigned char ucWord )
{
	int tint_high,tint_low;
	QString tstr_high,tstr_low;
	tint_high = ucWord/16;
	tint_low = ucWord%16;
	tstr_high = tstr_high.sprintf("%d",tint_high);
	tstr_low = tstr_low.sprintf("%d",tint_low);

	return tstr_high+QStringLiteral(".")+tstr_low;
}

QString ReadICINFOWorkThread::Transuchtoqstring( unsigned char ucWord )
{
	int tint_high,tint_low;
	QString tstr_high,tstr_low;
	QString tstr_res = "";
	tint_high = ucWord/16;
	tint_low = ucWord%16;
	switch(tint_high)
	{
	case 0:
		{
			tstr_high = "0";
			break;
		}
	case 1:
		{
			tstr_high = "1";
			break;
		}
	case 2:
		{
			tstr_high = "2";
			break;
		}
	case 3:
		{
			tstr_high = "3";
			break;
		}
	case 4:
		{
			tstr_high = "4";
			break;
		}
	case 5:
		{
			tstr_high = "5";
			break;
		}
	case 6:
		{
			tstr_high = "6";
			break;
		}
	case 7:
		{
			tstr_high = "7";
			break;
		}
	case 8:
		{
			tstr_high = "8";
			break;
		}
	case 9:
		{
			tstr_high = "9";
			break;
		}
	case 10:
		{
			tstr_high = "A";
			break;
		}
	case 11:
		{
			tstr_high = "B";
			break;
		}
	case 12:
		{
			tstr_high = "C";
			break;
		}
	case 13:
		{
			tstr_high = "D";
			break;
		}
	case 14:
		{
			tstr_high = "E";
			break;
		}
	case 15:
		{
			tstr_high = "F";
			break;
		}
	}

	switch(tint_low)
	{
	case 0:
		{
			tstr_low = "0";
			break;
		}
	case 1:
		{
			tstr_low = "1";
			break;
		}
	case 2:
		{
			tstr_low = "2";
			break;
		}
	case 3:
		{
			tstr_low = "3";
			break;
		}
	case 4:
		{
			tstr_low = "4";
			break;
		}
	case 5:
		{
			tstr_low = "5";
			break;
		}
	case 6:
		{
			tstr_low = "6";
			break;
		}
	case 7:
		{
			tstr_low = "7";
			break;
		}
	case 8:
		{
			tstr_low = "8";
			break;
		}
	case 9:
		{
			tstr_low = "9";
			break;
		}
	case 10:
		{
			tstr_low = "A";
			break;
		}
	case 11:
		{
			tstr_low = "B";
			break;
		}
	case 12:
		{
			tstr_low = "C";
			break;
		}
	case 13:
		{
			tstr_low = "D";
			break;
		}
	case 14:
		{
			tstr_low = "E";
			break;
		}
	case 15:
		{
			tstr_low = "F";
			break;
		}
	}

	tstr_res = tstr_high+tstr_low;

	return tstr_res;
}

void ReadICINFOWorkThread::StopThread( void )
{
	m_flag = Stop;
}

bool ReadICINFOWorkThread::converttohexstr( QString str,unsigned char* uchhex )
{
	str = str.toUpper();

	if (str.size()%2!=0){
		return false;
	}

	QChar* t_Qchar = new QChar[str.size()];
	t_Qchar = str.data();
	char* t_char = new char[str.size()];
	for (int i=0;i<str.size();i++){
		t_char[i] = t_Qchar[i].toLatin1();
	}

	for (int i=0;i<str.size();i++){
		if (t_char[i]>60){
			t_char[i] = t_char[i]-55;
		}
		else {
			t_char[i] = t_char[i]-48;
		}
	}

	for (int i=0;i<str.size()/2;i++){
		uchhex[i] = t_char[2*i]*16+t_char[2*i+1];
	}

	return true;
}
