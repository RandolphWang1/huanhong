//////////////////////////////////////////////////////////////////////////////
//20120413
//ymm
//////////////////////////////////////////////////////////////////////////////

#include <Main.h>

//////////////////////////////////////////////////////////////////////////////
int Support();
int SupportSelectApp();
int SelectApp(BYTE_PTR pNum);
int SupportAckApp();
int AckApp();
int Enable();
int TopApp(BYTE_PTR pNum);
int SendSelect(BYTE nNum);
int DeleteApp(BYTE nNum);
//============================================================================
// 应用选择
int AppSelect()
{
	INT nNum = 0 ;

	while(1)
	{
		// 是否双方都支持的应用
		if (ERROR == Support())
		{
			// 终止交易
			return ERROR;
		}
		// 终端是否支持持卡人选择应用
		if (ERROR != SupportSelectApp())
		{
			// 显示应用
			// 选择应用
			if (ERROR == SelectApp(&nNum))
			{
				// 终止交易
				return ERROR;
			}
			// 发送SELECT命令
			goto LABLE_SELECT;
		}
		
		// 终端是否支持持卡人确认应用
		if (ERROR != SupportAckApp())
		{
			// 显示最高级别应用
			// 确认应用
			if (ERROR == AckApp())
			{
				// 移出应用
				DeleteApp(nNum);
				continue;
			}	
			// 发送SELECT命令
			goto LABLE_SELECT;
		}

		// 应用没有确认,是否允许选择
		if (ERROR == Enable())
		{
			// 终止交易
			return ERROR;
		}
		// 显示最高级别应用
		// 最高级别的应用无需确认应用
		TopApp(&nNum);

	LBALE_SELECT:
		// SendSelect
		if (ERROR == SendSelect(nNum))
		{
			// 移出应用
			DeleteApp(nNum);
			continue;
		}
		
		break;
	}

	return OK;
}

//////////////////////////////////////////////////////////////////////////////

int GetProOpt();
int AnalyGPOData();
//============================================================================
int AppInit()
{
	// 条件是否满足
	if (ERROR == GetProOpt())
	{
		DeleteApp(nNum);
		// 返回应用列表	
		return ERROR;
	}
	
	if (ERROR == AnalyGPOData())
	{
		return ERROR;
	}
	return OK;
}

//////////////////////////////////////////////////////////////////////////////

//============================================================================
int ReadAppData()
{
	// 终端从AFL中选择第一个文件入口
	if (ERROR == OptFile())
	{
		return ERROR;
	}
	
	LABLE_MID:
	if (ERROR == ReadFileRecord())
	{
		return ERROR
	}

	if (ERROR != SDA())
	{
		InputSDAData();
	}

	if (ERROR == TellEndItem())
	{
		goto LABLE_MID;
	}

	if (ERROR != OtherEnter())
	{
		OptEnterNext();
		goto LABLE_MID;
	}

	return OK;
}

//////////////////////////////////////////////////////////////////////////////

//============================================================================
int DA(BYTE_PTR tvr)
{
	// 通过使用RID和PKI
	if (ERROR == GetPKByCA())
	{
		goto LABLE_DA;
	}
	
	// 使用PK从SAD获取数据
	if (ERROR == GetHashBySAD())
	{
		goto LABLE_DA;
	}
	// Compare hash
	if (ERROR == ComHashValue())
	{
		goto LABLE_DA;
	}

	return OK;
	
	LABLE_DA:
		*tvr == 1;
		return ERROR;
}
//////////////////////////////////////////////////////////////////////////////

//============================================================================
int ProLimit(BYTE_PTR tvr)
{
	if (ERROR == AppVerNum())
	{
		goto LABLE_NEXT;
	}

	if (ERROR == CompVerNum())
	{
		*tvr = 1;
	}
	
LABLE_NEXT:
	if (ERROR != AppCode())
	{
		if (ERROR == Limit())
		{
			*tvr = 1;
		}
	}

	if (ERROR == ableDate())
	{
			*tvr = 1;
	}

	if (ERROR == EnableDate())
	{
			*tvr = 1;
	}
	
	return OK;			
}

//////////////////////////////////////////////////////////////////////////////

//============================================================================
int Cardholder()
{
	if (ERROR == CardSupport())
	{
		
	}
	
}

