#ifndef _MB_FUNC_H
#define _MB_FUNC_H

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif
#if MB_FUNC_OTHER_REP_SLAVEID_BUF > 0
    eMBException eMBFuncReportSlaveID( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_READ_INPUT_ENABLED > 0
eMBException    eMBFuncReadInputRegister( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_READ_HOLDING_ENABLED > 0
eMBException    eMBFuncReadHoldingRegister( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_WRITE_HOLDING_ENABLED > 0
eMBException    eMBFuncWriteHoldingRegister( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
eMBException    eMBFuncWriteMultipleHoldingRegister( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_READ_COILS_ENABLED > 0
eMBException    eMBFuncReadCoils( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_WRITE_COIL_ENABLED > 0
eMBException    eMBFuncWriteCoil( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
eMBException    eMBFuncWriteMultipleCoils( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
eMBException    eMBFuncReadDiscreteInputs( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_READWRITE_HOLDING_ENABLED > 0
eMBException    eMBFuncReadWriteMultipleHoldingRegister( UCHAR * pucFrame, USHORT * usLen );
#endif

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
