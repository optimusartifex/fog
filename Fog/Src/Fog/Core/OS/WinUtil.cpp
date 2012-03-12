// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/OS/FilePath.h>
#include <Fog/Core/OS/WinUtil.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringTmp_p.h>
#include <Fog/Core/Tools/TextCodec.h>

// [Fix]
#if !defined(PATH_MAX)
# define PATH_MAX 1024
#endif // PATH_MAX

#if !defined(SM_SERVERR2)
# define SM_SERVERR2 89
#endif // SM_SERVERR2

namespace Fog {

// ============================================================================
// [Fog::WinUtil - Error Translation table]
// ============================================================================

// Error ranges used by Win-API:
//
// - 00000 -> 01299 System.
// - 01300 -> 01399 Security.
// - 01400 -> 01499 WinUser.
// - 01500 -> 01549 Event-Log.
// - 01550 -> 01599 Class Scheduler.
// - 01600 -> 01699 MSI.
// - 01700 -> 01999 RPC.
// - 02000 -> 02009 OpenGL.
// - 02010 -> 02049 Image Color Management.
// - 02100 -> 02999 Winnet32.
// - 03000 -> 03049 Win32 Spooler.
// - 03050 -> 03199 Reserved.
// - 03200 -> 03299 Isolation.
// - 03300 -> 03899 Reserved.
// - 03900 -> 03999 IO.
// - 04000 -> 04049 WinS.
// - 04050 -> 04099 PeerDist.
// - 04100 -> 04149 DHCP.
// - 04150 -> 04199 Reserved.
// - 04200 -> 04249 WMI Error codes.
// - 04250 -> 04299 Reserved.
// - 04300 -> 04349 RSM.
// - 04350 -> 04389 RSS.
// - 04390 -> 04399 Reparse Point.
// - 04400 -> 04499 Reserved.
// - 04500 -> 04549 Single Instance Store (SIS).
// - 04550 -> 04599 Reserved.
// - 05000 -> 05999 Cluster.
// - 06000 -> 06099 EFS.
// - 06100 -> 06199 Browser.
// - 06200 -> 06249 Task Scheduler.
// - 06250 -> 06599 Reserved.
// - 06600 -> 06699 Common Log (CLFS).
// - 06700 -> 06799 Transaction (KTM).
// - 06800 -> 06899 Transactional File Services.
// - 06900 -> 06999 Reserved.
// - 07000 -> 07099 Terminal Server.
// - 07100 -> 07499 Reserved.
// - 07500 -> 07999 Traffic Control.
// - 08000 -> 08999 Active Directory.
// - 09000 -> 09999 DNS.
// - 10000 -> 11999 WinSock.
// - 12000 -> 12999 Reserved.
// - 13000 -> 13999 IPSec.
// - 14000 -> 14999 Side By Side.
// - 15000 -> 15079 WinEvt.
// - 15080 -> 15099 Wecsvc.
// - 15100 -> 15199 MUI.
// - 15200 -> 15249 Monitor Configuration API.
// - 15250 -> 15299 Syspart.
// - 15300 -> 15320 Vortex.
// - ..... -> ..... COM

// For us the interesting ranges are:
// - 00000 -> 00500 System.
// - 01299 -> 01399 Security.
// - 01400 -> 01499 WinUser.
// - 02000 -> 02009 OpenGL.
// - 02010 -> 02049 Image Color Management.

#define _MAP(_Err_) ((_Err_) - _ERR_RANGE_FIRST)
#define _UNASSIGNED 0xFFFFU

static const uint16_t WinError_table_0_to_353[] =
{
// Never accessed.
/* 00000: ERROR_SUCCESS                                                 */ _UNASSIGNED,

/* 00001: ERROR_INVALID_FUNCTION                                        */ _UNASSIGNED,
/* 00002: ERROR_FILE_NOT_FOUND                                          */ _MAP(ERR_FILE_NOT_FOUND),
/* 00003: ERROR_PATH_NOT_FOUND                                          */ _MAP(ERR_PATH_NOT_FOUND),
/* 00004: ERROR_TOO_MANY_OPEN_FILES                                     */ _MAP(ERR_TOO_MANY_FILES),
/* 00005: ERROR_ACCESS_DENIED                                           */ _MAP(ERR_RT_ACCESS_DENIED),
/* 00006: ERROR_INVALID_HANDLE                                          */ _MAP(ERR_RT_INVALID_HANDLE),
/* 00007: ERROR_ARENA_TRASHED                                           */ _UNASSIGNED,
/* 00008: ERROR_NOT_ENOUGH_MEMORY                                       */ _MAP(ERR_RT_OUT_OF_MEMORY),
/* 00009: ERROR_INVALID_BLOCK                                           */ _UNASSIGNED,
/* 00010: ERROR_BAD_ENVIRONMENT                                         */ _UNASSIGNED,
/* 00011: ERROR_BAD_FORMAT                                              */ _UNASSIGNED,
/* 00012: ERROR_INVALID_ACCESS                                          */ _UNASSIGNED,
/* 00013: ERROR_INVALID_DATA                                            */ _UNASSIGNED,
/* 00014: ERROR_OUTOFMEMORY                                             */ _MAP(ERR_RT_OUT_OF_MEMORY),
/* 00015: ERROR_INVALID_DRIVE                                           */ _UNASSIGNED,
/* 00016: ERROR_CURRENT_DIRECTORY                                       */ _MAP(ERR_DIRECTORY_IN_USE),
/* 00017: ERROR_NOT_SAME_DEVICE                                         */ _MAP(ERR_DEVICE_MISMATCH),
/* 00018: ERROR_NO_MORE_FILES                                           */ _UNASSIGNED,
/* 00019: ERROR_WRITE_PROTECT                                           */ _MAP(ERR_DEVICE_READ_ONLY),
/* 00020: ERROR_BAD_UNIT                                                */ _UNASSIGNED,
/* 00021: ERROR_NOT_READY                                               */ _MAP(ERR_DEVICE_NOT_READY),
/* 00022: ERROR_BAD_COMMAND                                             */ _UNASSIGNED,
/* 00023: ERROR_CRC                                                     */ _MAP(ERR_DEVICE_FAILURE),
/* 00024: ERROR_BAD_LENGTH                                              */ _UNASSIGNED,
/* 00025: ERROR_SEEK                                                    */ _MAP(ERR_SEEK_OUT_OF_RANGE),
/* 00026: ERROR_NOT_DOS_DISK                                            */ _UNASSIGNED,
/* 00027: ERROR_SECTOR_NOT_FOUND                                        */ _UNASSIGNED,
/* 00028: ERROR_OUT_OF_PAPER                                            */ _UNASSIGNED,
/* 00029: ERROR_WRITE_FAULT                                             */ _MAP(ERR_DEVICE_FAILURE),
/* 00030: ERROR_READ_FAULT                                              */ _MAP(ERR_DEVICE_FAILURE),
/* 00031: ERROR_GEN_FAILURE                                             */ _MAP(ERR_DEVICE_FAILURE),
/* 00032: ERROR_SHARING_VIOLATION                                       */ _MAP(ERR_FILE_LOCKED),
/* 00033: ERROR_LOCK_VIOLATION                                          */ _MAP(ERR_FILE_LOCKED),
/* 00034: ERROR_WRONG_DISK                                              */ _UNASSIGNED,
/* 00035:                                                               */ _UNASSIGNED,
/* 00036: ERROR_SHARING_BUFFER_EXCEEDED                                 */ _UNASSIGNED,
/* 00037:                                                               */ _UNASSIGNED,
/* 00038: ERROR_HANDLE_EOF                                              */ _MAP(ERR_FILE_END),
/* 00039: ERROR_HANDLE_DISK_FULL                                        */ _MAP(ERR_DEVICE_FULL),
/* 00040:                                                               */ _UNASSIGNED,
/* 00041:                                                               */ _UNASSIGNED,
/* 00042:                                                               */ _UNASSIGNED,
/* 00043:                                                               */ _UNASSIGNED,
/* 00044:                                                               */ _UNASSIGNED,
/* 00045:                                                               */ _UNASSIGNED,
/* 00046:                                                               */ _UNASSIGNED,
/* 00047:                                                               */ _UNASSIGNED,
/* 00048:                                                               */ _UNASSIGNED,
/* 00049:                                                               */ _UNASSIGNED,
/* 00050: ERROR_NOT_SUPPORTED                                           */ _UNASSIGNED,
/* 00051: ERROR_REM_NOT_LIST                                            */ _UNASSIGNED,
/* 00052: ERROR_DUP_NAME                                                */ _UNASSIGNED,
/* 00053: ERROR_BAD_NETPATH                                             */ _UNASSIGNED,
/* 00054: ERROR_NETWORK_BUSY                                            */ _UNASSIGNED,
/* 00055: ERROR_DEV_NOT_EXIST                                           */ _UNASSIGNED,
/* 00056: ERROR_TOO_MANY_CMDS                                           */ _UNASSIGNED,
/* 00057: ERROR_ADAP_HDW_ERR                                            */ _UNASSIGNED,
/* 00058: ERROR_BAD_NET_RESP                                            */ _UNASSIGNED,
/* 00059: ERROR_UNEXP_NET_ERR                                           */ _UNASSIGNED,
/* 00060: ERROR_BAD_REM_ADAP                                            */ _UNASSIGNED,
/* 00061: ERROR_PRINTQ_FULL                                             */ _UNASSIGNED,
/* 00062: ERROR_NO_SPOOL_SPACE                                          */ _UNASSIGNED,
/* 00063: ERROR_PRINT_CANCELLED                                         */ _UNASSIGNED,
/* 00064: ERROR_NETNAME_DELETED                                         */ _UNASSIGNED,
/* 00065: ERROR_NETWORK_ACCESS_DENIED                                   */ _UNASSIGNED,
/* 00066: ERROR_BAD_DEV_TYPE                                            */ _UNASSIGNED,
/* 00067: ERROR_BAD_NET_NAME                                            */ _UNASSIGNED,
/* 00068: ERROR_TOO_MANY_NAMES                                          */ _UNASSIGNED,
/* 00069: ERROR_TOO_MANY_SESS                                           */ _UNASSIGNED,
/* 00070: ERROR_SHARING_PAUSED                                          */ _UNASSIGNED,
/* 00071: ERROR_REQ_NOT_ACCEP                                           */ _UNASSIGNED,
/* 00072: ERROR_REDIR_PAUSED                                            */ _UNASSIGNED,
/* 00073:                                                               */ _UNASSIGNED,
/* 00074:                                                               */ _UNASSIGNED,
/* 00075:                                                               */ _UNASSIGNED,
/* 00076:                                                               */ _UNASSIGNED,
/* 00077:                                                               */ _UNASSIGNED,
/* 00078:                                                               */ _UNASSIGNED,
/* 00079:                                                               */ _UNASSIGNED,
/* 00080: ERROR_FILE_EXISTS                                             */ _MAP(ERR_PATH_EXISTS),
/* 00081:                                                               */ _UNASSIGNED,
/* 00082: ERROR_CANNOT_MAKE                                             */ _MAP(ERR_IO_CANT_CREATE),
/* 00083: ERROR_FAIL_I24                                                */ _UNASSIGNED,
/* 00084: ERROR_OUT_OF_STRUCTURES                                       */ _UNASSIGNED,
/* 00085: ERROR_ALREADY_ASSIGNED                                        */ _UNASSIGNED,
/* 00086: ERROR_INVALID_PASSWORD                                        */ _UNASSIGNED,
/* 00087: ERROR_INVALID_PARAMETER                                       */ _MAP(ERR_RT_INVALID_ARGUMENT),
/* 00088: ERROR_NET_WRITE_FAULT                                         */ _UNASSIGNED,
/* 00089: ERROR_NO_PROC_SLOTS                                           */ _MAP(ERR_RT_OUT_OF_PROCESSES),
/* 00090:                                                               */ _UNASSIGNED,
/* 00091:                                                               */ _UNASSIGNED,
/* 00092:                                                               */ _UNASSIGNED,
/* 00093:                                                               */ _UNASSIGNED,
/* 00094:                                                               */ _UNASSIGNED,
/* 00095:                                                               */ _UNASSIGNED,
/* 00096:                                                               */ _UNASSIGNED,
/* 00097:                                                               */ _UNASSIGNED,
/* 00098:                                                               */ _UNASSIGNED,
/* 00099:                                                               */ _UNASSIGNED,
/* 00100: ERROR_TOO_MANY_SEMAPHORES                                     */ _MAP(ERR_RT_OUT_OF_SEMAPHORES),
/* 00101: ERROR_EXCL_SEM_ALREADY_OWNED                                  */ _UNASSIGNED,
/* 00102: ERROR_SEM_IS_SET                                              */ _UNASSIGNED,
/* 00103: ERROR_TOO_MANY_SEM_REQUESTS                                   */ _UNASSIGNED,
/* 00104: ERROR_INVALID_AT_INTERRUPT_TIME                               */ _UNASSIGNED,
/* 00105: ERROR_SEM_OWNER_DIED                                          */ _UNASSIGNED,
/* 00106: ERROR_SEM_USER_LIMIT                                          */ _UNASSIGNED,
/* 00107: ERROR_DISK_CHANGE                                             */ _UNASSIGNED,
/* 00108: ERROR_DRIVE_LOCKED                                            */ _MAP(ERR_DEVICE_LOCKED),
/* 00109: ERROR_BROKEN_PIPE                                             */ _MAP(ERR_PIPE_END),
/* 00110: ERROR_OPEN_FAILED                                             */ _MAP(ERR_IO_CANT_OPEN),
/* 00111: ERROR_BUFFER_OVERFLOW                                         */ _UNASSIGNED,
/* 00112: ERROR_DISK_FULL                                               */ _MAP(ERR_DEVICE_FULL),
/* 00113: ERROR_NO_MORE_SEARCH_HANDLES                                  */ _UNASSIGNED,
/* 00114: ERROR_INVALID_TARGET_HANDLE                                   */ _UNASSIGNED,
/* 00115:                                                               */ _UNASSIGNED,
/* 00116:                                                               */ _UNASSIGNED,
/* 00117: ERROR_INVALID_CATEGORY                                        */ _UNASSIGNED,
/* 00118: ERROR_INVALID_VERIFY_SWITCH                                   */ _UNASSIGNED,
/* 00119: ERROR_BAD_DRIVER_LEVEL                                        */ _UNASSIGNED,
/* 00120: ERROR_CALL_NOT_IMPLEMENTED                                    */ _UNASSIGNED,
/* 00121: ERROR_SEM_TIMEOUT                                             */ _UNASSIGNED,
/* 00122: ERROR_INSUFFICIENT_BUFFER                                     */ _UNASSIGNED,
/* 00123: ERROR_INVALID_NAME                                            */ _UNASSIGNED,
/* 00124: ERROR_INVALID_LEVEL                                           */ _UNASSIGNED,
/* 00125: ERROR_NO_VOLUME_LABEL                                         */ _UNASSIGNED,
/* 00126: ERROR_MOD_NOT_FOUND                                           */ _UNASSIGNED,
/* 00127: ERROR_PROC_NOT_FOUND                                          */ _UNASSIGNED,
/* 00128: ERROR_WAIT_NO_CHILDREN                                        */ _UNASSIGNED,
/* 00129: ERROR_CHILD_NOT_COMPLETE                                      */ _UNASSIGNED,
/* 00130: ERROR_DIRECT_ACCESS_HANDLE                                    */ _UNASSIGNED,
/* 00131: ERROR_NEGATIVE_SEEK                                           */ _MAP(ERR_SEEK_OUT_OF_RANGE),
/* 00132: ERROR_SEEK_ON_DEVICE                                          */ _MAP(ERR_SEEK_OUT_OF_RANGE),
/* 00133: ERROR_IS_JOIN_TARGET                                          */ _UNASSIGNED,
/* 00134: ERROR_IS_JOINED                                               */ _UNASSIGNED,
/* 00135: ERROR_IS_SUBSTED                                              */ _UNASSIGNED,
/* 00136: ERROR_NOT_JOINED                                              */ _UNASSIGNED,
/* 00137: ERROR_NOT_SUBSTED                                             */ _UNASSIGNED,
/* 00138: ERROR_JOIN_TO_JOIN                                            */ _UNASSIGNED,
/* 00139: ERROR_SUBST_TO_SUBST                                          */ _UNASSIGNED,
/* 00140: ERROR_JOIN_TO_SUBST                                           */ _UNASSIGNED,
/* 00141: ERROR_SUBST_TO_JOIN                                           */ _UNASSIGNED,
/* 00142: ERROR_BUSY_DRIVE                                              */ _MAP(ERR_RT_BUSY),
/* 00143: ERROR_SAME_DRIVE                                              */ _UNASSIGNED,
/* 00144: ERROR_DIR_NOT_ROOT                                            */ _MAP(ERR_DIRECTORY_NOT_ROOT),
/* 00145: ERROR_DIR_NOT_EMPTY                                           */ _MAP(ERR_DIRECTORY_NOT_EMPTY),
/* 00146: ERROR_IS_SUBST_PATH                                           */ _UNASSIGNED,
/* 00147: ERROR_IS_JOIN_PATH                                            */ _UNASSIGNED,
/* 00148: ERROR_PATH_BUSY                                               */ _MAP(ERR_RT_BUSY),
/* 00149: ERROR_IS_SUBST_TARGET                                         */ _UNASSIGNED,
/* 00150: ERROR_SYSTEM_TRACE                                            */ _UNASSIGNED,
/* 00151: ERROR_INVALID_EVENT_COUNT                                     */ _UNASSIGNED,
/* 00152: ERROR_TOO_MANY_MUXWAITERS                                     */ _UNASSIGNED,
/* 00153: ERROR_INVALID_LIST_FORMAT                                     */ _UNASSIGNED,
/* 00154: ERROR_LABEL_TOO_LONG                                          */ _UNASSIGNED,
/* 00155: ERROR_TOO_MANY_TCBS                                           */ _MAP(ERR_RT_OUT_OF_THREADS),
/* 00156: ERROR_SIGNAL_REFUSED                                          */ _UNASSIGNED,
/* 00157: ERROR_DISCARDED                                               */ _UNASSIGNED,
/* 00158: ERROR_NOT_LOCKED                                              */ _UNASSIGNED,
/* 00159: ERROR_BAD_THREADID_ADDR                                       */ _UNASSIGNED,
/* 00160: ERROR_BAD_ARGUMENTS                                           */ _MAP(ERR_RT_INVALID_ARGUMENT),
/* 00161: ERROR_BAD_PATHNAME                                            */ _UNASSIGNED,
/* 00162: ERROR_SIGNAL_PENDING                                          */ _UNASSIGNED,
/* 00163:                                                               */ _UNASSIGNED,
/* 00164: ERROR_MAX_THRDS_REACHED                                       */ _MAP(ERR_RT_OUT_OF_THREADS),
/* 00165:                                                               */ _UNASSIGNED,
/* 00166:                                                               */ _UNASSIGNED,
/* 00167: ERROR_LOCK_FAILED                                             */ _UNASSIGNED,
/* 00168:                                                               */ _UNASSIGNED,
/* 00169:                                                               */ _UNASSIGNED,
/* 00170: ERROR_BUSY                                                    */ _MAP(ERR_RT_BUSY),
/* 00171:                                                               */ _UNASSIGNED,
/* 00172:                                                               */ _UNASSIGNED,
/* 00173: ERROR_CANCEL_VIOLATION                                        */ _UNASSIGNED,
/* 00174: ERROR_ATOMIC_LOCKS_NOT_SUPPORTED                              */ _MAP(ERR_DEVICE_NOT_ATOMIC),
/* 00175:                                                               */ _UNASSIGNED,
/* 00176:                                                               */ _UNASSIGNED,
/* 00177:                                                               */ _UNASSIGNED,
/* 00178:                                                               */ _UNASSIGNED,
/* 00179:                                                               */ _UNASSIGNED,
/* 00180: ERROR_INVALID_SEGMENT_NUMBER                                  */ _UNASSIGNED,
/* 00181:                                                               */ _UNASSIGNED,
/* 00182: ERROR_INVALID_ORDINAL                                         */ _UNASSIGNED,
/* 00183: ERROR_ALREADY_EXISTS                                          */ _MAP(ERR_PATH_EXISTS),
/* 00184:                                                               */ _UNASSIGNED,
/* 00185:                                                               */ _UNASSIGNED,
/* 00186: ERROR_INVALID_FLAG_NUMBER                                     */ _UNASSIGNED,
/* 00187: ERROR_SEM_NOT_FOUND                                           */ _UNASSIGNED,
/* 00188: ERROR_INVALID_STARTING_CODESEG                                */ _UNASSIGNED,
/* 00189: ERROR_INVALID_STACKSEG                                        */ _UNASSIGNED,
/* 00190: ERROR_INVALID_MODULETYPE                                      */ _UNASSIGNED,
/* 00191: ERROR_INVALID_EXE_SIGNATURE                                   */ _UNASSIGNED,
/* 00192: ERROR_EXE_MARKED_INVALID                                      */ _UNASSIGNED,
/* 00193: ERROR_BAD_EXE_FORMAT                                          */ _UNASSIGNED,
/* 00194: ERROR_ITERATED_DATA_EXCEEDS_64k                               */ _UNASSIGNED,
/* 00195: ERROR_INVALID_MINALLOCSIZE                                    */ _UNASSIGNED,
/* 00196: ERROR_DYNLINK_FROM_INVALID_RING                               */ _UNASSIGNED,
/* 00197: ERROR_IOPL_NOT_ENABLED                                        */ _UNASSIGNED,
/* 00198: ERROR_INVALID_SEGDPL                                          */ _UNASSIGNED,
/* 00199: ERROR_AUTODATASEG_EXCEEDS_64k                                 */ _UNASSIGNED,
/* 00200: ERROR_RING2SEG_MUST_BE_MOVABLE                                */ _UNASSIGNED,
/* 00201: ERROR_RELOC_CHAIN_XEEDS_SEGLIM                                */ _UNASSIGNED,
/* 00202: ERROR_INFLOOP_IN_RELOC_CHAIN                                  */ _UNASSIGNED,
/* 00203: ERROR_ENVVAR_NOT_FOUND                                        */ _MAP(ERR_ENVIRONMENT_NOT_FOUND),
/* 00204:                                                               */ _UNASSIGNED,
/* 00205: ERROR_NO_SIGNAL_SENT                                          */ _UNASSIGNED,
/* 00206: ERROR_FILENAME_EXCED_RANGE                                    */ _UNASSIGNED,
/* 00207: ERROR_RING2_STACK_IN_USE                                      */ _UNASSIGNED,
/* 00208: ERROR_META_EXPANSION_TOO_LONG                                 */ _UNASSIGNED,
/* 00209: ERROR_INVALID_SIGNAL_NUMBER                                   */ _UNASSIGNED,
/* 00210: ERROR_THREAD_1_INACTIVE                                       */ _UNASSIGNED,
/* 00211:                                                               */ _UNASSIGNED,
/* 00212: ERROR_LOCKED                                                  */ _UNASSIGNED,
/* 00213:                                                               */ _UNASSIGNED,
/* 00214: ERROR_TOO_MANY_MODULES                                        */ _MAP(ERR_LIBRARY_TOO_MANY),
/* 00215: ERROR_NESTING_NOT_ALLOWED                                     */ _UNASSIGNED,
/* 00216: ERROR_EXE_MACHINE_TYPE_MISMATCH                               */ _UNASSIGNED,
/* 00217: ERROR_EXE_CANNOT_MODIFY_SIGNED_BINARY                         */ _UNASSIGNED,
/* 00218: ERROR_EXE_CANNOT_MODIFY_STRONG_SIGNED_BINARY                  */ _UNASSIGNED,
/* 00219:                                                               */ _UNASSIGNED,
/* 00220: ERROR_FILE_CHECKED_OUT                                        */ _UNASSIGNED,
/* 00221: ERROR_CHECKOUT_REQUIRED                                       */ _UNASSIGNED,
/* 00222: ERROR_BAD_FILE_TYPE                                           */ _UNASSIGNED,
/* 00223: ERROR_FILE_TOO_LARGE                                          */ _MAP(ERR_FILE_TOO_LARGE),
/* 00224: ERROR_FORMS_AUTH_REQUIRED                                     */ _UNASSIGNED,
/* 00225: ERROR_VIRUS_INFECTED                                          */ _UNASSIGNED,
/* 00226: ERROR_VIRUS_DELETED                                           */ _UNASSIGNED,
/* 00227:                                                               */ _UNASSIGNED,
/* 00228:                                                               */ _UNASSIGNED,
/* 00229: ERROR_PIPE_LOCAL                                              */ _MAP(ERR_PIPE_LOCAL),
/* 00230: ERROR_BAD_PIPE                                                */ _MAP(ERR_PIPE_INVALID),
/* 00231: ERROR_PIPE_BUSY                                               */ _MAP(ERR_PIPE_BUSY),
/* 00232: ERROR_NO_DATA                                                 */ _MAP(ERR_PIPE_EMPTY),
/* 00233: ERROR_PIPE_NOT_CONNECTED                                      */ _MAP(ERR_PIPE_DISCONNECTED),
/* 00234: ERROR_MORE_DATA                                               */ _UNASSIGNED,
/* 00235:                                                               */ _UNASSIGNED,
/* 00236:                                                               */ _UNASSIGNED,
/* 00237:                                                               */ _UNASSIGNED,
/* 00238:                                                               */ _UNASSIGNED,
/* 00239:                                                               */ _UNASSIGNED,
/* 00240: ERROR_VC_DISCONNECTED                                         */ _UNASSIGNED,
/* 00241:                                                               */ _UNASSIGNED,
/* 00242:                                                               */ _UNASSIGNED,
/* 00243:                                                               */ _UNASSIGNED,
/* 00244:                                                               */ _UNASSIGNED,
/* 00245:                                                               */ _UNASSIGNED,
/* 00246:                                                               */ _UNASSIGNED,
/* 00247:                                                               */ _UNASSIGNED,
/* 00248:                                                               */ _UNASSIGNED,
/* 00249:                                                               */ _UNASSIGNED,
/* 00250:                                                               */ _UNASSIGNED,
/* 00251:                                                               */ _UNASSIGNED,
/* 00252:                                                               */ _UNASSIGNED,
/* 00253:                                                               */ _UNASSIGNED,
/* 00254: ERROR_INVALID_EA_NAME                                         */ _UNASSIGNED,
/* 00255: ERROR_EA_LIST_INCONSISTENT                                    */ _UNASSIGNED,
/* 00256:                                                               */ _UNASSIGNED,
/* 00257:                                                               */ _UNASSIGNED,
/* 00258:                                                               */ _UNASSIGNED,
/* 00259: ERROR_NO_MORE_ITEMS                                           */ _UNASSIGNED,
/* 00260:                                                               */ _UNASSIGNED,
/* 00261:                                                               */ _UNASSIGNED,
/* 00262:                                                               */ _UNASSIGNED,
/* 00263:                                                               */ _UNASSIGNED,
/* 00264:                                                               */ _UNASSIGNED,
/* 00265:                                                               */ _UNASSIGNED,
/* 00266: ERROR_CANNOT_COPY                                             */ _UNASSIGNED,
/* 00267: ERROR_DIRECTORY                                               */ _MAP(ERR_PATH_NAME_INVALID),
/* 00268:                                                               */ _UNASSIGNED,
/* 00269:                                                               */ _UNASSIGNED,
/* 00270:                                                               */ _UNASSIGNED,
/* 00271:                                                               */ _UNASSIGNED,
/* 00272:                                                               */ _UNASSIGNED,
/* 00273:                                                               */ _UNASSIGNED,
/* 00274:                                                               */ _UNASSIGNED,
/* 00275: ERROR_EAS_DIDNT_FIT                                           */ _UNASSIGNED,
/* 00276: ERROR_EA_FILE_CORRUPT                                         */ _UNASSIGNED,
/* 00277: ERROR_EA_TABLE_FULL                                           */ _UNASSIGNED,
/* 00278: ERROR_INVALID_EA_HANDLE                                       */ _UNASSIGNED,
/* 00279:                                                               */ _UNASSIGNED,
/* 00280:                                                               */ _UNASSIGNED,
/* 00281:                                                               */ _UNASSIGNED,
/* 00282: ERROR_EAS_NOT_SUPPORTED                                       */ _UNASSIGNED,
/* 00283:                                                               */ _UNASSIGNED,
/* 00284:                                                               */ _UNASSIGNED,
/* 00285:                                                               */ _UNASSIGNED,
/* 00286:                                                               */ _UNASSIGNED,
/* 00287:                                                               */ _UNASSIGNED,
/* 00288: ERROR_NOT_OWNER                                               */ _UNASSIGNED,
/* 00289:                                                               */ _UNASSIGNED,
/* 00290:                                                               */ _UNASSIGNED,
/* 00291:                                                               */ _UNASSIGNED,
/* 00292:                                                               */ _UNASSIGNED,
/* 00293:                                                               */ _UNASSIGNED,
/* 00294:                                                               */ _UNASSIGNED,
/* 00295:                                                               */ _UNASSIGNED,
/* 00296:                                                               */ _UNASSIGNED,
/* 00297:                                                               */ _UNASSIGNED,
/* 00298: ERROR_TOO_MANY_POSTS                                          */ _UNASSIGNED,
/* 00299: ERROR_PARTIAL_COPY                                            */ _UNASSIGNED,
/* 00300: ERROR_OPLOCK_NOT_GRANTED                                      */ _UNASSIGNED,
/* 00301: ERROR_INVALID_OPLOCK_PROTOCOL                                 */ _UNASSIGNED,
/* 00302: ERROR_DISK_TOO_FRAGMENTED                                     */ _MAP(ERR_DEVICE_FRAGMENTED),
/* 00303: ERROR_DELETE_PENDING                                          */ _UNASSIGNED,
/* 00304: ERROR_INCOMPATIBLE_WITH_GLOBAL_SHORT_NAME_REGISTRY_SETTING    */ _UNASSIGNED,
/* 00305: ERROR_SHORT_NAMES_NOT_ENABLED_ON_VOLUME                       */ _UNASSIGNED,
/* 00306: ERROR_SECURITY_STREAM_IS_INCONSISTENT                         */ _UNASSIGNED,
/* 00307: ERROR_INVALID_LOCK_RANGE                                      */ _UNASSIGNED,
/* 00308: ERROR_IMAGE_SUBSYSTEM_NOT_PRESENT                             */ _UNASSIGNED,
/* 00309: ERROR_NOTIFICATION_GUID_ALREADY_DEFINED                       */ _UNASSIGNED,
/* 00310:                                                               */ _UNASSIGNED,
/* 00311:                                                               */ _UNASSIGNED,
/* 00312:                                                               */ _UNASSIGNED,
/* 00313:                                                               */ _UNASSIGNED,
/* 00314:                                                               */ _UNASSIGNED,
/* 00315:                                                               */ _UNASSIGNED,
/* 00316:                                                               */ _UNASSIGNED,
/* 00317: ERROR_MR_MID_NOT_FOUND                                        */ _UNASSIGNED,
/* 00318: ERROR_SCOPE_NOT_FOUND                                         */ _UNASSIGNED,
/* 00319:                                                               */ _UNASSIGNED,
/* 00320:                                                               */ _UNASSIGNED,
/* 00321:                                                               */ _UNASSIGNED,
/* 00322:                                                               */ _UNASSIGNED,
/* 00323:                                                               */ _UNASSIGNED,
/* 00324:                                                               */ _UNASSIGNED,
/* 00325:                                                               */ _UNASSIGNED,
/* 00326:                                                               */ _UNASSIGNED,
/* 00327:                                                               */ _UNASSIGNED,
/* 00328:                                                               */ _UNASSIGNED,
/* 00329:                                                               */ _UNASSIGNED,
/* 00330:                                                               */ _UNASSIGNED,
/* 00331:                                                               */ _UNASSIGNED,
/* 00332:                                                               */ _UNASSIGNED,
/* 00333:                                                               */ _UNASSIGNED,
/* 00334:                                                               */ _UNASSIGNED,
/* 00335:                                                               */ _UNASSIGNED,
/* 00336:                                                               */ _UNASSIGNED,
/* 00337:                                                               */ _UNASSIGNED,
/* 00338:                                                               */ _UNASSIGNED,
/* 00339:                                                               */ _UNASSIGNED,
/* 00340:                                                               */ _UNASSIGNED,
/* 00341:                                                               */ _UNASSIGNED,
/* 00342:                                                               */ _UNASSIGNED,
/* 00343:                                                               */ _UNASSIGNED,
/* 00344:                                                               */ _UNASSIGNED,
/* 00345:                                                               */ _UNASSIGNED,
/* 00346:                                                               */ _UNASSIGNED,
/* 00347:                                                               */ _UNASSIGNED,
/* 00348:                                                               */ _UNASSIGNED,
/* 00349:                                                               */ _UNASSIGNED,
/* 00350: ERROR_FAIL_NOACTION_REBOOT                                    */ _UNASSIGNED,
/* 00351: ERROR_FAIL_SHUTDOWN                                           */ _UNASSIGNED,
/* 00352: ERROR_FAIL_RESTART                                            */ _UNASSIGNED,
/* 00353: ERROR_MAX_SESSIONS_REACHED                                    */ _UNASSIGNED
};

static const uint16_t WinError_table_400_to_403[] =
{
/* 00400: ERROR_THREAD_MODE_ALREADY_BACKGROUND                          */ _UNASSIGNED,
/* 00401: ERROR_THREAD_MODE_NOT_BACKGROUND                              */ _UNASSIGNED,
/* 00402: ERROR_PROCESS_MODE_ALREADY_BACKGROUND                         */ _UNASSIGNED,
/* 00403: ERROR_PROCESS_MODE_NOT_BACKGROUND                             */ _UNASSIGNED
};

static const uint16_t WinError_table_534_to_805[] =
{
/* 00534: ERROR_ARITHMETIC_OVERFLOW                                     */ _MAP(ERR_RT_OVERFLOW),
/* 00535: ERROR_PIPE_CONNECTED                                          */ _UNASSIGNED,
/* 00536: ERROR_PIPE_LISTENING                                          */ _UNASSIGNED,
/* 00537: ERROR_VERIFIER_STOP                                           */ _UNASSIGNED,
/* 00538: ERROR_ABIOS_ERROR                                             */ _UNASSIGNED,
/* 00539: ERROR_WX86_WARNING                                            */ _UNASSIGNED,
/* 00540: ERROR_WX86_ERROR                                              */ _UNASSIGNED,
/* 00541: ERROR_TIMER_NOT_CANCELED                                      */ _UNASSIGNED,
/* 00542: ERROR_UNWIND                                                  */ _UNASSIGNED,
/* 00543: ERROR_BAD_STACK                                               */ _UNASSIGNED,
/* 00544: ERROR_INVALID_UNWIND_TARGET                                   */ _UNASSIGNED,
/* 00545: ERROR_INVALID_PORT_ATTRIBUTES                                 */ _UNASSIGNED,
/* 00546: ERROR_PORT_MESSAGE_TOO_LONG                                   */ _UNASSIGNED,
/* 00547: ERROR_INVALID_QUOTA_LOWER                                     */ _UNASSIGNED,
/* 00548: ERROR_DEVICE_ALREADY_ATTACHED                                 */ _UNASSIGNED,
/* 00549: ERROR_INSTRUCTION_MISALIGNMENT                                */ _UNASSIGNED,
/* 00550: ERROR_PROFILING_NOT_STARTED                                   */ _UNASSIGNED,
/* 00551: ERROR_PROFILING_NOT_STOPPED                                   */ _UNASSIGNED,
/* 00552: ERROR_COULD_NOT_INTERPRET                                     */ _UNASSIGNED,
/* 00553: ERROR_PROFILING_AT_LIMIT                                      */ _UNASSIGNED,
/* 00554: ERROR_CANT_WAIT                                               */ _UNASSIGNED,
/* 00555: ERROR_CANT_TERMINATE_SELF                                     */ _UNASSIGNED,
/* 00556: ERROR_UNEXPECTED_MM_CREATE_ERR                                */ _UNASSIGNED,
/* 00557: ERROR_UNEXPECTED_MM_MAP_ERROR                                 */ _UNASSIGNED,
/* 00558: ERROR_UNEXPECTED_MM_EXTEND_ERR                                */ _UNASSIGNED,
/* 00559: ERROR_BAD_FUNCTION_TABLE                                      */ _UNASSIGNED,
/* 00560: ERROR_NO_GUID_TRANSLATION                                     */ _UNASSIGNED,
/* 00561: ERROR_INVALID_LDT_SIZE                                        */ _UNASSIGNED,
/* 00562:                                                               */ _UNASSIGNED,
/* 00563: ERROR_INVALID_LDT_OFFSET                                      */ _UNASSIGNED,
/* 00564: ERROR_INVALID_LDT_DESCRIPTOR                                  */ _UNASSIGNED,
/* 00565: ERROR_TOO_MANY_THREADS                                        */ _UNASSIGNED,
/* 00566: ERROR_THREAD_NOT_IN_PROCESS                                   */ _UNASSIGNED,
/* 00567: ERROR_PAGEFILE_QUOTA_EXCEEDED                                 */ _UNASSIGNED,
/* 00568: ERROR_LOGON_SERVER_CONFLICT                                   */ _UNASSIGNED,
/* 00569: ERROR_SYNCHRONIZATION_REQUIRED                                */ _UNASSIGNED,
/* 00570: ERROR_NET_OPEN_FAILED                                         */ _UNASSIGNED,
/* 00571: ERROR_IO_PRIVILEGE_FAILED                                     */ _UNASSIGNED,
/* 00572: ERROR_CONTROL_C_EXIT                                          */ _UNASSIGNED,
/* 00573: ERROR_MISSING_SYSTEMFILE                                      */ _UNASSIGNED,
/* 00574: ERROR_UNHANDLED_EXCEPTION                                     */ _UNASSIGNED,
/* 00575: ERROR_APP_INIT_FAILURE                                        */ _UNASSIGNED,
/* 00576: ERROR_PAGEFILE_CREATE_FAILED                                  */ _UNASSIGNED,
/* 00577: ERROR_INVALID_IMAGE_HASH                                      */ _UNASSIGNED,
/* 00578: ERROR_NO_PAGEFILE                                             */ _UNASSIGNED,
/* 00579: ERROR_ILLEGAL_FLOAT_CONTEXT                                   */ _UNASSIGNED,
/* 00580: ERROR_NO_EVENT_PAIR                                           */ _UNASSIGNED,
/* 00581: ERROR_DOMAIN_CTRLR_CONFIG_ERROR                               */ _UNASSIGNED,
/* 00582: ERROR_ILLEGAL_CHARACTER                                       */ _UNASSIGNED,
/* 00583: ERROR_UNDEFINED_CHARACTER                                     */ _UNASSIGNED,
/* 00584: ERROR_FLOPPY_VOLUME                                           */ _UNASSIGNED,
/* 00585: ERROR_BIOS_FAILED_TO_CONNECT_INTERRUPT                        */ _UNASSIGNED,
/* 00586: ERROR_BACKUP_CONTROLLER                                       */ _UNASSIGNED,
/* 00587: ERROR_MUTANT_LIMIT_EXCEEDED                                   */ _UNASSIGNED,
/* 00588: ERROR_FS_DRIVER_REQUIRED                                      */ _UNASSIGNED,
/* 00589: ERROR_CANNOT_LOAD_REGISTRY_FILE                               */ _UNASSIGNED,
/* 00590: ERROR_DEBUG_ATTACH_FAILED                                     */ _UNASSIGNED,
/* 00591: ERROR_SYSTEM_PROCESS_TERMINATED                               */ _UNASSIGNED,
/* 00592: ERROR_DATA_NOT_ACCEPTED                                       */ _UNASSIGNED,
/* 00593: ERROR_VDM_HARD_ERROR                                          */ _UNASSIGNED,
/* 00594: ERROR_DRIVER_CANCEL_TIMEOUT                                   */ _UNASSIGNED,
/* 00595: ERROR_REPLY_MESSAGE_MISMATCH                                  */ _UNASSIGNED,
/* 00596: ERROR_LOST_WRITEBEHIND_DATA                                   */ _UNASSIGNED,
/* 00597: ERROR_CLIENT_SERVER_PARAMETERS_INVALID                        */ _UNASSIGNED,
/* 00598: ERROR_NOT_TINY_STREAM                                         */ _UNASSIGNED,
/* 00599: ERROR_STACK_OVERFLOW_READ                                     */ _UNASSIGNED,
/* 00600: ERROR_CONVERT_TO_LARGE                                        */ _UNASSIGNED,
/* 00601: ERROR_FOUND_OUT_OF_SCOPE                                      */ _UNASSIGNED,
/* 00602: ERROR_ALLOCATE_BUCKET                                         */ _UNASSIGNED,
/* 00603: ERROR_MARSHALL_OVERFLOW                                       */ _UNASSIGNED,
/* 00604: ERROR_INVALID_VARIANT                                         */ _UNASSIGNED,
/* 00605: ERROR_BAD_COMPRESSION_BUFFER                                  */ _UNASSIGNED,
/* 00606: ERROR_AUDIT_FAILED                                            */ _UNASSIGNED,
/* 00607: ERROR_TIMER_RESOLUTION_NOT_SET                                */ _UNASSIGNED,
/* 00608: ERROR_INSUFFICIENT_LOGON_INFO                                 */ _UNASSIGNED,
/* 00609: ERROR_BAD_DLL_ENTRYPOINT                                      */ _UNASSIGNED,
/* 00610: ERROR_BAD_SERVICE_ENTRYPOINT                                  */ _UNASSIGNED,
/* 00611: ERROR_IP_ADDRESS_CONFLICT1                                    */ _UNASSIGNED,
/* 00612: ERROR_IP_ADDRESS_CONFLICT2                                    */ _UNASSIGNED,
/* 00613: ERROR_REGISTRY_QUOTA_LIMIT                                    */ _UNASSIGNED,
/* 00614: ERROR_NO_CALLBACK_ACTIVE                                      */ _UNASSIGNED,
/* 00615: ERROR_PWD_TOO_SHORT                                           */ _UNASSIGNED,
/* 00616: ERROR_PWD_TOO_RECENT                                          */ _UNASSIGNED,
/* 00617: ERROR_PWD_HISTORY_CONFLICT                                    */ _UNASSIGNED,
/* 00618: ERROR_UNSUPPORTED_COMPRESSION                                 */ _UNASSIGNED,
/* 00619: ERROR_INVALID_HW_PROFILE                                      */ _UNASSIGNED,
/* 00620: ERROR_INVALID_PLUGPLAY_DEVICE_PATH                            */ _UNASSIGNED,
/* 00621: ERROR_QUOTA_LIST_INCONSISTENT                                 */ _UNASSIGNED,
/* 00622: ERROR_EVALUATION_EXPIRATION                                   */ _UNASSIGNED,
/* 00623: ERROR_ILLEGAL_DLL_RELOCATION                                  */ _UNASSIGNED,
/* 00624: ERROR_DLL_INIT_FAILED_LOGOFF                                  */ _UNASSIGNED,
/* 00625: ERROR_VALIDATE_CONTINUE                                       */ _UNASSIGNED,
/* 00626: ERROR_NO_MORE_MATCHES                                         */ _UNASSIGNED,
/* 00627: ERROR_RANGE_LIST_CONFLICT                                     */ _UNASSIGNED,
/* 00628: ERROR_SERVER_SID_MISMATCH                                     */ _UNASSIGNED,
/* 00629: ERROR_CANT_ENABLE_DENY_ONLY                                   */ _UNASSIGNED,
/* 00630: ERROR_FLOAT_MULTIPLE_FAULTS                                   */ _UNASSIGNED,
/* 00631: ERROR_FLOAT_MULTIPLE_TRAPS                                    */ _UNASSIGNED,
/* 00632: ERROR_NOINTERFACE                                             */ _UNASSIGNED,
/* 00633: ERROR_DRIVER_FAILED_SLEEP                                     */ _UNASSIGNED,
/* 00634: ERROR_CORRUPT_SYSTEM_FILE                                     */ _UNASSIGNED,
/* 00635: ERROR_COMMITMENT_MINIMUM                                      */ _UNASSIGNED,
/* 00636: ERROR_PNP_RESTART_ENUMERATION                                 */ _UNASSIGNED,
/* 00637: ERROR_SYSTEM_IMAGE_BAD_SIGNATURE                              */ _UNASSIGNED,
/* 00638: ERROR_PNP_REBOOT_REQUIRED                                     */ _UNASSIGNED,
/* 00639: ERROR_INSUFFICIENT_POWER                                      */ _UNASSIGNED,
/* 00640: ERROR_MULTIPLE_FAULT_VIOLATION                                */ _UNASSIGNED,
/* 00641: ERROR_SYSTEM_SHUTDOWN                                         */ _UNASSIGNED,
/* 00642: ERROR_PORT_NOT_SET                                            */ _UNASSIGNED,
/* 00643: ERROR_DS_VERSION_CHECK_FAILURE                                */ _UNASSIGNED,
/* 00644: ERROR_RANGE_NOT_FOUND                                         */ _UNASSIGNED,
/* 00645:                                                               */ _UNASSIGNED,
/* 00646: ERROR_NOT_SAFE_MODE_DRIVER                                    */ _UNASSIGNED,
/* 00647: ERROR_FAILED_DRIVER_ENTRY                                     */ _UNASSIGNED,
/* 00648: ERROR_DEVICE_ENUMERATION_ERROR                                */ _UNASSIGNED,
/* 00649: ERROR_MOUNT_POINT_NOT_RESOLVED                                */ _UNASSIGNED,
/* 00650: ERROR_INVALID_DEVICE_OBJECT_PARAMETER                         */ _UNASSIGNED,
/* 00651: ERROR_MCA_OCCURED                                             */ _UNASSIGNED,
/* 00652: ERROR_DRIVER_DATABASE_ERROR                                   */ _UNASSIGNED,
/* 00653: ERROR_SYSTEM_HIVE_TOO_LARGE                                   */ _UNASSIGNED,
/* 00654: ERROR_DRIVER_FAILED_PRIOR_UNLOAD                              */ _UNASSIGNED,
/* 00655: ERROR_VOLSNAP_PREPARE_HIBERNATE                               */ _UNASSIGNED,
/* 00656: ERROR_HIBERNATION_FAILURE                                     */ _UNASSIGNED,
/* 00657:                                                               */ _UNASSIGNED,
/* 00658:                                                               */ _UNASSIGNED,
/* 00659:                                                               */ _UNASSIGNED,
/* 00660:                                                               */ _UNASSIGNED,
/* 00661:                                                               */ _UNASSIGNED,
/* 00662:                                                               */ _UNASSIGNED,
/* 00663:                                                               */ _UNASSIGNED,
/* 00664:                                                               */ _UNASSIGNED,
/* 00665: ERROR_FILE_SYSTEM_LIMITATION                                  */ _UNASSIGNED,
/* 00666:                                                               */ _UNASSIGNED,
/* 00667:                                                               */ _UNASSIGNED,
/* 00668: ERROR_ASSERTION_FAILURE                                       */ _MAP(ERR_RT_ASSERTION_FAILURE),
/* 00669: ERROR_ACPI_ERROR                                              */ _UNASSIGNED,
/* 00670: ERROR_WOW_ASSERTION                                           */ _MAP(ERR_RT_ASSERTION_FAILURE),
/* 00671: ERROR_PNP_BAD_MPS_TABLE                                       */ _UNASSIGNED,
/* 00672: ERROR_PNP_TRANSLATION_FAILED                                  */ _UNASSIGNED,
/* 00673: ERROR_PNP_IRQ_TRANSLATION_FAILED                              */ _UNASSIGNED,
/* 00674: ERROR_PNP_INVALID_ID                                          */ _UNASSIGNED,
/* 00675: ERROR_WAKE_SYSTEM_DEBUGGER                                    */ _UNASSIGNED,
/* 00676: ERROR_HANDLES_CLOSED                                          */ _UNASSIGNED,
/* 00677: ERROR_EXTRANEOUS_INFORMATION                                  */ _UNASSIGNED,
/* 00678: ERROR_RXACT_COMMIT_NECESSARY                                  */ _UNASSIGNED,
/* 00679: ERROR_MEDIA_CHECK                                             */ _UNASSIGNED,
/* 00680: ERROR_GUID_SUBSTITUTION_MADE                                  */ _UNASSIGNED,
/* 00681: ERROR_STOPPED_ON_SYMLINK                                      */ _UNASSIGNED,
/* 00682: ERROR_LONGJUMP                                                */ _UNASSIGNED,
/* 00683: ERROR_PLUGPLAY_QUERY_VETOED                                   */ _UNASSIGNED,
/* 00684: ERROR_UNWIND_CONSOLIDATE                                      */ _UNASSIGNED,
/* 00685: ERROR_REGISTRY_HIVE_RECOVERED                                 */ _UNASSIGNED,
/* 00686: ERROR_DLL_MIGHT_BE_INSECURE                                   */ _UNASSIGNED,
/* 00687: ERROR_DLL_MIGHT_BE_INCOMPATIBLE                               */ _UNASSIGNED,
/* 00688: ERROR_DBG_EXCEPTION_NOT_HANDLED                               */ _UNASSIGNED,
/* 00689: ERROR_DBG_REPLY_LATER                                         */ _UNASSIGNED,
/* 00690: ERROR_DBG_UNABLE_TO_PROVIDE_HANDLE                            */ _UNASSIGNED,
/* 00691: ERROR_DBG_TERMINATE_THREAD                                    */ _UNASSIGNED,
/* 00692: ERROR_DBG_TERMINATE_PROCESS                                   */ _UNASSIGNED,
/* 00693: ERROR_DBG_CONTROL_C                                           */ _UNASSIGNED,
/* 00694: ERROR_DBG_PRINTEXCEPTION_C                                    */ _UNASSIGNED,
/* 00695: ERROR_DBG_RIPEXCEPTION                                        */ _UNASSIGNED,
/* 00696: ERROR_DBG_CONTROL_BREAK                                       */ _UNASSIGNED,
/* 00697: ERROR_DBG_COMMAND_EXCEPTION                                   */ _UNASSIGNED,
/* 00698: ERROR_OBJECT_NAME_EXISTS                                      */ _UNASSIGNED,
/* 00699: ERROR_THREAD_WAS_SUSPENDED                                    */ _UNASSIGNED,
/* 00700: ERROR_IMAGE_NOT_AT_BASE                                       */ _UNASSIGNED,
/* 00701: ERROR_RXACT_STATE_CREATED                                     */ _UNASSIGNED,
/* 00702: ERROR_SEGMENT_NOTIFICATION                                    */ _UNASSIGNED,
/* 00703: ERROR_BAD_CURRENT_DIRECTORY                                   */ _UNASSIGNED,
/* 00704: ERROR_FT_READ_RECOVERY_FROM_BACKUP                            */ _UNASSIGNED,
/* 00705: ERROR_FT_WRITE_RECOVERY                                       */ _UNASSIGNED,
/* 00706: ERROR_IMAGE_MACHINE_TYPE_MISMATCH                             */ _UNASSIGNED,
/* 00707: ERROR_RECEIVE_PARTIAL                                         */ _UNASSIGNED,
/* 00708: ERROR_RECEIVE_EXPEDITED                                       */ _UNASSIGNED,
/* 00709: ERROR_RECEIVE_PARTIAL_EXPEDITED                               */ _UNASSIGNED,
/* 00710: ERROR_EVENT_DONE                                              */ _UNASSIGNED,
/* 00711: ERROR_EVENT_PENDING                                           */ _UNASSIGNED,
/* 00712: ERROR_CHECKING_FILE_SYSTEM                                    */ _UNASSIGNED,
/* 00713: ERROR_FATAL_APP_EXIT                                          */ _UNASSIGNED,
/* 00714: ERROR_PREDEFINED_HANDLE                                       */ _UNASSIGNED,
/* 00715: ERROR_WAS_UNLOCKED                                            */ _UNASSIGNED,
/* 00716: ERROR_SERVICE_NOTIFICATION                                    */ _UNASSIGNED,
/* 00717: ERROR_WAS_LOCKED                                              */ _UNASSIGNED,
/* 00718: ERROR_LOG_HARD_ERROR                                          */ _UNASSIGNED,
/* 00719: ERROR_ALREADY_WIN32                                           */ _UNASSIGNED,
/* 00720: ERROR_IMAGE_MACHINE_TYPE_MISMATCH_EXE                         */ _UNASSIGNED,
/* 00721: ERROR_NO_YIELD_PERFORMED                                      */ _UNASSIGNED,
/* 00722: ERROR_TIMER_RESUME_IGNORED                                    */ _UNASSIGNED,
/* 00723: ERROR_ARBITRATION_UNHANDLED                                   */ _UNASSIGNED,
/* 00724: ERROR_CARDBUS_NOT_SUPPORTED                                   */ _UNASSIGNED,
/* 00725: ERROR_MP_PROCESSOR_MISMATCH                                   */ _UNASSIGNED,
/* 00726: ERROR_HIBERNATED                                              */ _UNASSIGNED,
/* 00727: ERROR_RESUME_HIBERNATION                                      */ _UNASSIGNED,
/* 00728: ERROR_FIRMWARE_UPDATED                                        */ _UNASSIGNED,
/* 00729: ERROR_DRIVERS_LEAKING_LOCKED_PAGES                            */ _UNASSIGNED,
/* 00730: ERROR_WAKE_SYSTEM                                             */ _UNASSIGNED,
/* 00731: ERROR_WAIT_1                                                  */ _UNASSIGNED,
/* 00732: ERROR_WAIT_2                                                  */ _UNASSIGNED,
/* 00733: ERROR_WAIT_3                                                  */ _UNASSIGNED,
/* 00734: ERROR_WAIT_63                                                 */ _UNASSIGNED,
/* 00735: ERROR_ABANDONED_WAIT_0                                        */ _UNASSIGNED,
/* 00736: ERROR_ABANDONED_WAIT_63                                       */ _UNASSIGNED,
/* 00737: ERROR_USER_APC                                                */ _UNASSIGNED,
/* 00738: ERROR_KERNEL_APC                                              */ _UNASSIGNED,
/* 00739: ERROR_ALERTED                                                 */ _UNASSIGNED,
/* 00740: ERROR_ELEVATION_REQUIRED                                      */ _UNASSIGNED,
/* 00741: ERROR_REPARSE                                                 */ _UNASSIGNED,
/* 00742: ERROR_OPLOCK_BREAK_IN_PROGRESS                                */ _UNASSIGNED,
/* 00743: ERROR_VOLUME_MOUNTED                                          */ _UNASSIGNED,
/* 00744: ERROR_RXACT_COMMITTED                                         */ _UNASSIGNED,
/* 00745: ERROR_NOTIFY_CLEANUP                                          */ _UNASSIGNED,
/* 00746: ERROR_PRIMARY_TRANSPORT_CONNECT_FAILED                        */ _UNASSIGNED,
/* 00747: ERROR_PAGE_FAULT_TRANSITION                                   */ _UNASSIGNED,
/* 00748: ERROR_PAGE_FAULT_DEMAND_ZERO                                  */ _UNASSIGNED,
/* 00749: ERROR_PAGE_FAULT_COPY_ON_WRITE                                */ _UNASSIGNED,
/* 00750: ERROR_PAGE_FAULT_GUARD_PAGE                                   */ _UNASSIGNED,
/* 00751: ERROR_PAGE_FAULT_PAGING_FILE                                  */ _UNASSIGNED,
/* 00752: ERROR_CACHE_PAGE_LOCKED                                       */ _UNASSIGNED,
/* 00753: ERROR_CRASH_DUMP                                              */ _UNASSIGNED,
/* 00754: ERROR_BUFFER_ALL_ZEROS                                        */ _UNASSIGNED,
/* 00755: ERROR_REPARSE_OBJECT                                          */ _UNASSIGNED,
/* 00756: ERROR_RESOURCE_REQUIREMENTS_CHANGED                           */ _UNASSIGNED,
/* 00757: ERROR_TRANSLATION_COMPLETE                                    */ _UNASSIGNED,
/* 00758: ERROR_NOTHING_TO_TERMINATE                                    */ _UNASSIGNED,
/* 00759: ERROR_PROCESS_NOT_IN_JOB                                      */ _UNASSIGNED,
/* 00760: ERROR_PROCESS_IN_JOB                                          */ _UNASSIGNED,
/* 00761: ERROR_VOLSNAP_HIBERNATE_READY                                 */ _UNASSIGNED,
/* 00762: ERROR_FSFILTER_OP_COMPLETED_SUCCESSFULLY                      */ _UNASSIGNED,
/* 00763: ERROR_INTERRUPT_VECTOR_ALREADY_CONNECTED                      */ _UNASSIGNED,
/* 00764: ERROR_INTERRUPT_STILL_CONNECTED                               */ _UNASSIGNED,
/* 00765: ERROR_WAIT_FOR_OPLOCK                                         */ _UNASSIGNED,
/* 00766: ERROR_DBG_EXCEPTION_HANDLED                                   */ _UNASSIGNED,
/* 00767: ERROR_DBG_CONTINUE                                            */ _UNASSIGNED,
/* 00768: ERROR_CALLBACK_POP_STACK                                      */ _UNASSIGNED,
/* 00769: ERROR_COMPRESSION_DISABLED                                    */ _UNASSIGNED,
/* 00770: ERROR_CANTFETCHBACKWARDS                                      */ _UNASSIGNED,
/* 00771: ERROR_CANTSCROLLBACKWARDS                                     */ _UNASSIGNED,
/* 00772: ERROR_ROWSNOTRELEASED                                         */ _UNASSIGNED,
/* 00773: ERROR_BAD_ACCESSOR_FLAGS                                      */ _UNASSIGNED,
/* 00774: ERROR_ERRORS_ENCOUNTERED                                      */ _UNASSIGNED,
/* 00775: ERROR_NOT_CAPABLE                                             */ _UNASSIGNED,
/* 00776: ERROR_REQUEST_OUT_OF_SEQUENCE                                 */ _UNASSIGNED,
/* 00777: ERROR_VERSION_PARSE_ERROR                                     */ _UNASSIGNED,
/* 00778: ERROR_BADSTARTPOSITION                                        */ _UNASSIGNED,
/* 00779: ERROR_MEMORY_HARDWARE                                         */ _UNASSIGNED,
/* 00780: ERROR_DISK_REPAIR_DISABLED                                    */ _UNASSIGNED,
/* 00781: ERROR_INSUFFICIENT_RESOURCE_FOR_SPECIFIED_SHARED_SECTION_SIZE */ _UNASSIGNED,
/* 00782: ERROR_SYSTEM_POWERSTATE_TRANSITION                            */ _UNASSIGNED,
/* 00783: ERROR_SYSTEM_POWERSTATE_COMPLEX_TRANSITION                    */ _UNASSIGNED,
/* 00784: ERROR_MCA_EXCEPTION                                           */ _UNASSIGNED,
/* 00785: ERROR_ACCESS_AUDIT_BY_POLICY                                  */ _UNASSIGNED,
/* 00786: ERROR_ACCESS_DISABLED_NO_SAFER_UI_BY_POLICY                   */ _UNASSIGNED,
/* 00787: ERROR_ABANDON_HIBERFILE                                       */ _UNASSIGNED,
/* 00788: ERROR_LOST_WRITEBEHIND_DATA_NETWORK_DISCONNECTED              */ _UNASSIGNED,
/* 00789: ERROR_LOST_WRITEBEHIND_DATA_NETWORK_SERVER_ERROR              */ _UNASSIGNED,
/* 00790: ERROR_LOST_WRITEBEHIND_DATA_LOCAL_DISK_ERROR                  */ _UNASSIGNED,
/* 00791: ERROR_BAD_MCFG_TABLE                                          */ _UNASSIGNED,
/* 00792:                                                               */ _UNASSIGNED,
/* 00793:                                                               */ _UNASSIGNED,
/* 00794:                                                               */ _UNASSIGNED,
/* 00795:                                                               */ _UNASSIGNED,
/* 00796:                                                               */ _UNASSIGNED,
/* 00797:                                                               */ _UNASSIGNED,
/* 00798:                                                               */ _UNASSIGNED,
/* 00799:                                                               */ _UNASSIGNED,
/* 00800: ERROR_OPLOCK_SWITCHED_TO_NEW_HANDLE                           */ _UNASSIGNED,
/* 00801: ERROR_CANNOT_GRANT_REQUESTED_OPLOCK                           */ _UNASSIGNED,
/* 00802: ERROR_CANNOT_BREAK_OPLOCK                                     */ _UNASSIGNED,
/* 00803: ERROR_OPLOCK_HANDLE_CLOSED                                    */ _UNASSIGNED,
/* 00804: ERROR_NO_ACE_CONDITION                                        */ _UNASSIGNED,
/* 00805: ERROR_INVALID_ACE_CONDITION                                   */ _UNASSIGNED
};

static const uint16_t WinError_table_994_to_1470[] =
{
/* 00994: ERROR_EA_ACCESS_DENIED                                        */ _UNASSIGNED,
/* 00995: ERROR_OPERATION_ABORTED                                       */ _UNASSIGNED,
/* 00996: ERROR_IO_INCOMPLETE                                           */ _UNASSIGNED,
/* 00997: ERROR_IO_PENDING                                              */ _UNASSIGNED,
/* 00998: ERROR_NOACCESS                                                */ _UNASSIGNED,
/* 00999: ERROR_SWAPERROR                                               */ _UNASSIGNED,
/* 01000:                                                               */ _UNASSIGNED,
/* 01001: ERROR_STACK_OVERFLOW                                          */ _UNASSIGNED,
/* 01002: ERROR_INVALID_MESSAGE                                         */ _UNASSIGNED,
/* 01003: ERROR_CAN_NOT_COMPLETE                                        */ _UNASSIGNED,
/* 01004: ERROR_INVALID_FLAGS                                           */ _UNASSIGNED,
/* 01005: ERROR_UNRECOGNIZED_VOLUME                                     */ _UNASSIGNED,
/* 01006: ERROR_FILE_INVALID                                            */ _UNASSIGNED,
/* 01007: ERROR_FULLSCREEN_MODE                                         */ _UNASSIGNED,
/* 01008: ERROR_NO_TOKEN                                                */ _UNASSIGNED,
/* 01009: ERROR_BADDB                                                   */ _UNASSIGNED,
/* 01010: ERROR_BADKEY                                                  */ _UNASSIGNED,
/* 01011: ERROR_CANTOPEN                                                */ _UNASSIGNED,
/* 01012: ERROR_CANTREAD                                                */ _UNASSIGNED,
/* 01013: ERROR_CANTWRITE                                               */ _UNASSIGNED,
/* 01014: ERROR_REGISTRY_RECOVERED                                      */ _UNASSIGNED,
/* 01015: ERROR_REGISTRY_CORRUPT                                        */ _UNASSIGNED,
/* 01016: ERROR_REGISTRY_IO_FAILED                                      */ _UNASSIGNED,
/* 01017: ERROR_NOT_REGISTRY_FILE                                       */ _UNASSIGNED,
/* 01018: ERROR_KEY_DELETED                                             */ _UNASSIGNED,
/* 01019: ERROR_NO_LOG_SPACE                                            */ _UNASSIGNED,
/* 01020: ERROR_KEY_HAS_CHILDREN                                        */ _UNASSIGNED,
/* 01021: ERROR_CHILD_MUST_BE_VOLATILE                                  */ _UNASSIGNED,
/* 01022: ERROR_NOTIFY_ENUM_DIR                                         */ _UNASSIGNED,
/* 01023:                                                               */ _UNASSIGNED,
/* 01024:                                                               */ _UNASSIGNED,
/* 01025:                                                               */ _UNASSIGNED,
/* 01026:                                                               */ _UNASSIGNED,
/* 01027:                                                               */ _UNASSIGNED,
/* 01028:                                                               */ _UNASSIGNED,
/* 01029:                                                               */ _UNASSIGNED,
/* 01030:                                                               */ _UNASSIGNED,
/* 01031:                                                               */ _UNASSIGNED,
/* 01032:                                                               */ _UNASSIGNED,
/* 01033:                                                               */ _UNASSIGNED,
/* 01034:                                                               */ _UNASSIGNED,
/* 01035:                                                               */ _UNASSIGNED,
/* 01036:                                                               */ _UNASSIGNED,
/* 01037:                                                               */ _UNASSIGNED,
/* 01038:                                                               */ _UNASSIGNED,
/* 01039:                                                               */ _UNASSIGNED,
/* 01040:                                                               */ _UNASSIGNED,
/* 01041:                                                               */ _UNASSIGNED,
/* 01042:                                                               */ _UNASSIGNED,
/* 01043:                                                               */ _UNASSIGNED,
/* 01044:                                                               */ _UNASSIGNED,
/* 01045:                                                               */ _UNASSIGNED,
/* 01046:                                                               */ _UNASSIGNED,
/* 01047:                                                               */ _UNASSIGNED,
/* 01048:                                                               */ _UNASSIGNED,
/* 01049:                                                               */ _UNASSIGNED,
/* 01050:                                                               */ _UNASSIGNED,
/* 01051: ERROR_DEPENDENT_SERVICES_RUNNING                              */ _UNASSIGNED,
/* 01052: ERROR_INVALID_SERVICE_CONTROL                                 */ _UNASSIGNED,
/* 01053: ERROR_SERVICE_REQUEST_TIMEOUT                                 */ _UNASSIGNED,
/* 01054: ERROR_SERVICE_NO_THREAD                                       */ _UNASSIGNED,
/* 01055: ERROR_SERVICE_DATABASE_LOCKED                                 */ _UNASSIGNED,
/* 01056: ERROR_SERVICE_ALREADY_RUNNING                                 */ _UNASSIGNED,
/* 01057: ERROR_INVALID_SERVICE_ACCOUNT                                 */ _UNASSIGNED,
/* 01058: ERROR_SERVICE_DISABLED                                        */ _UNASSIGNED,
/* 01059: ERROR_CIRCULAR_DEPENDENCY                                     */ _UNASSIGNED,
/* 01060: ERROR_SERVICE_DOES_NOT_EXIST                                  */ _UNASSIGNED,
/* 01061: ERROR_SERVICE_CANNOT_ACCEPT_CTRL                              */ _UNASSIGNED,
/* 01062: ERROR_SERVICE_NOT_ACTIVE                                      */ _UNASSIGNED,
/* 01063: ERROR_FAILED_SERVICE_CONTROLLER_CONNECT                       */ _UNASSIGNED,
/* 01064: ERROR_EXCEPTION_IN_SERVICE                                    */ _UNASSIGNED,
/* 01065: ERROR_DATABASE_DOES_NOT_EXIST                                 */ _UNASSIGNED,
/* 01066: ERROR_SERVICE_SPECIFIC_ERROR                                  */ _UNASSIGNED,
/* 01067: ERROR_PROCESS_ABORTED                                         */ _UNASSIGNED,
/* 01068: ERROR_SERVICE_DEPENDENCY_FAIL                                 */ _UNASSIGNED,
/* 01069: ERROR_SERVICE_LOGON_FAILED                                    */ _UNASSIGNED,
/* 01070: ERROR_SERVICE_START_HANG                                      */ _UNASSIGNED,
/* 01071: ERROR_INVALID_SERVICE_LOCK                                    */ _UNASSIGNED,
/* 01072: ERROR_SERVICE_MARKED_FOR_DELETE                               */ _UNASSIGNED,
/* 01073: ERROR_SERVICE_EXISTS                                          */ _UNASSIGNED,
/* 01074: ERROR_ALREADY_RUNNING_LKG                                     */ _UNASSIGNED,
/* 01075: ERROR_SERVICE_DEPENDENCY_DELETED                              */ _UNASSIGNED,
/* 01076: ERROR_BOOT_ALREADY_ACCEPTED                                   */ _UNASSIGNED,
/* 01077: ERROR_SERVICE_NEVER_STARTED                                   */ _UNASSIGNED,
/* 01078: ERROR_DUPLICATE_SERVICE_NAME                                  */ _UNASSIGNED,
/* 01079: ERROR_DIFFERENT_SERVICE_ACCOUNT                               */ _UNASSIGNED,
/* 01080: ERROR_CANNOT_DETECT_DRIVER_FAILURE                            */ _UNASSIGNED,
/* 01081: ERROR_CANNOT_DETECT_PROCESS_ABORT                             */ _UNASSIGNED,
/* 01082: ERROR_NO_RECOVERY_PROGRAM                                     */ _UNASSIGNED,
/* 01083: ERROR_SERVICE_NOT_IN_EXE                                      */ _UNASSIGNED,
/* 01084: ERROR_NOT_SAFEBOOT_SERVICE                                    */ _UNASSIGNED,
/* 01085:                                                               */ _UNASSIGNED,
/* 01086:                                                               */ _UNASSIGNED,
/* 01087:                                                               */ _UNASSIGNED,
/* 01088:                                                               */ _UNASSIGNED,
/* 01089:                                                               */ _UNASSIGNED,
/* 01090:                                                               */ _UNASSIGNED,
/* 01091:                                                               */ _UNASSIGNED,
/* 01092:                                                               */ _UNASSIGNED,
/* 01093:                                                               */ _UNASSIGNED,
/* 01094:                                                               */ _UNASSIGNED,
/* 01095:                                                               */ _UNASSIGNED,
/* 01096:                                                               */ _UNASSIGNED,
/* 01097:                                                               */ _UNASSIGNED,
/* 01098:                                                               */ _UNASSIGNED,
/* 01099:                                                               */ _UNASSIGNED,
/* 01100: ERROR_END_OF_MEDIA                                            */ _UNASSIGNED,
/* 01101: ERROR_FILEMARK_DETECTED                                       */ _UNASSIGNED,
/* 01102: ERROR_BEGINNING_OF_MEDIA                                      */ _UNASSIGNED,
/* 01103: ERROR_SETMARK_DETECTED                                        */ _UNASSIGNED,
/* 01104: ERROR_NO_DATA_DETECTED                                        */ _UNASSIGNED,
/* 01105: ERROR_PARTITION_FAILURE                                       */ _UNASSIGNED,
/* 01106: ERROR_INVALID_BLOCK_LENGTH                                    */ _UNASSIGNED,
/* 01107: ERROR_DEVICE_NOT_PARTITIONED                                  */ _UNASSIGNED,
/* 01108: ERROR_UNABLE_TO_LOCK_MEDIA                                    */ _UNASSIGNED,
/* 01109: ERROR_UNABLE_TO_UNLOAD_MEDIA                                  */ _UNASSIGNED,
/* 01110: ERROR_MEDIA_CHANGED                                           */ _UNASSIGNED,
/* 01111: ERROR_BUS_RESET                                               */ _UNASSIGNED,
/* 01112: ERROR_NO_MEDIA_IN_DRIVE                                       */ _MAP(ERR_DEVICE_NOT_READY),
/* 01113: ERROR_NO_UNICODE_TRANSLATION                                  */ _UNASSIGNED,
/* 01114: ERROR_DLL_INIT_FAILED                                         */ _UNASSIGNED,
/* 01115: ERROR_SHUTDOWN_IN_PROGRESS                                    */ _UNASSIGNED,
/* 01116: ERROR_NO_SHUTDOWN_IN_PROGRESS                                 */ _UNASSIGNED,
/* 01117: ERROR_IO_DEVICE                                               */ _UNASSIGNED,
/* 01118: ERROR_SERIAL_NO_DEVICE                                        */ _UNASSIGNED,
/* 01119: ERROR_IRQ_BUSY                                                */ _UNASSIGNED,
/* 01120: ERROR_MORE_WRITES                                             */ _UNASSIGNED,
/* 01121: ERROR_COUNTER_TIMEOUT                                         */ _UNASSIGNED,
/* 01122: ERROR_FLOPPY_ID_MARK_NOT_FOUND                                */ _UNASSIGNED,
/* 01123: ERROR_FLOPPY_WRONG_CYLINDER                                   */ _UNASSIGNED,
/* 01124: ERROR_FLOPPY_UNKNOWN_ERROR                                    */ _UNASSIGNED,
/* 01125: ERROR_FLOPPY_BAD_REGISTERS                                    */ _UNASSIGNED,
/* 01126: ERROR_DISK_RECALIBRATE_FAILED                                 */ _UNASSIGNED,
/* 01127: ERROR_DISK_OPERATION_FAILED                                   */ _UNASSIGNED,
/* 01128: ERROR_DISK_RESET_FAILED                                       */ _UNASSIGNED,
/* 01129: ERROR_EOM_OVERFLOW                                            */ _UNASSIGNED,
/* 01130: ERROR_NOT_ENOUGH_SERVER_MEMORY                                */ _MAP(ERR_RT_OUT_OF_MEMORY),
/* 01131: ERROR_POSSIBLE_DEADLOCK                                       */ _MAP(ERR_RT_DEADLOCK),
/* 01132: ERROR_MAPPED_ALIGNMENT                                        */ _UNASSIGNED,
/* 01133:                                                               */ _UNASSIGNED,
/* 01134:                                                               */ _UNASSIGNED,
/* 01135:                                                               */ _UNASSIGNED,
/* 01136:                                                               */ _UNASSIGNED,
/* 01137:                                                               */ _UNASSIGNED,
/* 01138:                                                               */ _UNASSIGNED,
/* 01139:                                                               */ _UNASSIGNED,
/* 01140: ERROR_SET_POWER_STATE_VETOED                                  */ _UNASSIGNED,
/* 01141: ERROR_SET_POWER_STATE_FAILED                                  */ _UNASSIGNED,
/* 01142: ERROR_TOO_MANY_LINKS                                          */ _MAP(ERR_TOO_MANY_LINKS),
/* 01143:                                                               */ _UNASSIGNED,
/* 01144:                                                               */ _UNASSIGNED,
/* 01145:                                                               */ _UNASSIGNED,
/* 01146:                                                               */ _UNASSIGNED,
/* 01147:                                                               */ _UNASSIGNED,
/* 01148:                                                               */ _UNASSIGNED,
/* 01149:                                                               */ _UNASSIGNED,
/* 01150: ERROR_OLD_WIN_VERSION                                         */ _UNASSIGNED,
/* 01151: ERROR_APP_WRONG_OS                                            */ _UNASSIGNED,
/* 01152: ERROR_SINGLE_INSTANCE_APP                                     */ _UNASSIGNED,
/* 01153: ERROR_RMODE_APP                                               */ _UNASSIGNED,
/* 01154: ERROR_INVALID_DLL                                             */ _UNASSIGNED,
/* 01155: ERROR_NO_ASSOCIATION                                          */ _UNASSIGNED,
/* 01156: ERROR_DDE_FAIL                                                */ _UNASSIGNED,
/* 01157: ERROR_DLL_NOT_FOUND                                           */ _UNASSIGNED,
/* 01158: ERROR_NO_MORE_USER_HANDLES                                    */ _UNASSIGNED,
/* 01159: ERROR_MESSAGE_SYNC_ONLY                                       */ _UNASSIGNED,
/* 01160: ERROR_SOURCE_ELEMENT_EMPTY                                    */ _UNASSIGNED,
/* 01161: ERROR_DESTINATION_ELEMENT_FULL                                */ _UNASSIGNED,
/* 01162: ERROR_ILLEGAL_ELEMENT_ADDRESS                                 */ _UNASSIGNED,
/* 01163: ERROR_MAGAZINE_NOT_PRESENT                                    */ _UNASSIGNED,
/* 01164: ERROR_DEVICE_REINITIALIZATION_NEEDED                          */ _UNASSIGNED,
/* 01165: ERROR_DEVICE_REQUIRES_CLEANING                                */ _UNASSIGNED,
/* 01166: ERROR_DEVICE_DOOR_OPEN                                        */ _UNASSIGNED,
/* 01167: ERROR_DEVICE_NOT_CONNECTED                                    */ _UNASSIGNED,
/* 01168: ERROR_NOT_FOUND                                               */ _UNASSIGNED,
/* 01169: ERROR_NO_MATCH                                                */ _UNASSIGNED,
/* 01170: ERROR_SET_NOT_FOUND                                           */ _UNASSIGNED,
/* 01171: ERROR_POINT_NOT_FOUND                                         */ _UNASSIGNED,
/* 01172: ERROR_NO_TRACKING_SERVICE                                     */ _UNASSIGNED,
/* 01173: ERROR_NO_VOLUME_ID                                            */ _UNASSIGNED,
/* 01174:                                                               */ _UNASSIGNED,
/* 01175: ERROR_UNABLE_TO_REMOVE_REPLACED                               */ _UNASSIGNED,
/* 01176: ERROR_UNABLE_TO_MOVE_REPLACEMENT                              */ _UNASSIGNED,
/* 01177: ERROR_UNABLE_TO_MOVE_REPLACEMENT_2                            */ _UNASSIGNED,
/* 01178: ERROR_JOURNAL_DELETE_IN_PROGRESS                              */ _UNASSIGNED,
/* 01179: ERROR_JOURNAL_NOT_ACTIVE                                      */ _UNASSIGNED,
/* 01180: ERROR_POTENTIAL_FILE_FOUND                                    */ _UNASSIGNED,
/* 01181: ERROR_JOURNAL_ENTRY_DELETED                                   */ _UNASSIGNED,
/* 01182:                                                               */ _UNASSIGNED,
/* 01183:                                                               */ _UNASSIGNED,
/* 01184:                                                               */ _UNASSIGNED,
/* 01185:                                                               */ _UNASSIGNED,
/* 01186:                                                               */ _UNASSIGNED,
/* 01187:                                                               */ _UNASSIGNED,
/* 01188:                                                               */ _UNASSIGNED,
/* 01189:                                                               */ _UNASSIGNED,
/* 01190: ERROR_SHUTDOWN_IS_SCHEDULED                                   */ _UNASSIGNED,
/* 01191: ERROR_SHUTDOWN_USERS_LOGGED_ON                                */ _UNASSIGNED,
/* 01192:                                                               */ _UNASSIGNED,
/* 01193:                                                               */ _UNASSIGNED,
/* 01194:                                                               */ _UNASSIGNED,
/* 01195:                                                               */ _UNASSIGNED,
/* 01196:                                                               */ _UNASSIGNED,
/* 01197:                                                               */ _UNASSIGNED,
/* 01198:                                                               */ _UNASSIGNED,
/* 01199:                                                               */ _UNASSIGNED,
/* 01200: ERROR_BAD_DEVICE                                              */ _UNASSIGNED,
/* 01201: ERROR_CONNECTION_UNAVAIL                                      */ _UNASSIGNED,
/* 01202: ERROR_DEVICE_ALREADY_REMEMBERED                               */ _UNASSIGNED,
/* 01203: ERROR_NO_NET_OR_BAD_PATH                                      */ _UNASSIGNED,
/* 01204: ERROR_BAD_PROVIDER                                            */ _UNASSIGNED,
/* 01205: ERROR_CANNOT_OPEN_PROFILE                                     */ _UNASSIGNED,
/* 01206: ERROR_BAD_PROFILE                                             */ _UNASSIGNED,
/* 01207: ERROR_NOT_CONTAINER                                           */ _UNASSIGNED,
/* 01208: ERROR_EXTENDED_ERROR                                          */ _UNASSIGNED,
/* 01209: ERROR_INVALID_GROUPNAME                                       */ _UNASSIGNED,
/* 01210: ERROR_INVALID_COMPUTERNAME                                    */ _UNASSIGNED,
/* 01211: ERROR_INVALID_EVENTNAME                                       */ _UNASSIGNED,
/* 01212: ERROR_INVALID_DOMAINNAME                                      */ _UNASSIGNED,
/* 01213: ERROR_INVALID_SERVICENAME                                     */ _UNASSIGNED,
/* 01214: ERROR_INVALID_NETNAME                                         */ _UNASSIGNED,
/* 01215: ERROR_INVALID_SHARENAME                                       */ _UNASSIGNED,
/* 01216: ERROR_INVALID_PASSWORDNAME                                    */ _UNASSIGNED,
/* 01217: ERROR_INVALID_MESSAGENAME                                     */ _UNASSIGNED,
/* 01218: ERROR_INVALID_MESSAGEDEST                                     */ _UNASSIGNED,
/* 01219: ERROR_SESSION_CREDENTIAL_CONFLICT                             */ _UNASSIGNED,
/* 01220: ERROR_REMOTE_SESSION_LIMIT_EXCEEDED                           */ _UNASSIGNED,
/* 01221: ERROR_DUP_DOMAINNAME                                          */ _UNASSIGNED,
/* 01222: ERROR_NO_NETWORK                                              */ _UNASSIGNED,
/* 01223: ERROR_CANCELLED                                               */ _UNASSIGNED,
/* 01224: ERROR_USER_MAPPED_FILE                                        */ _UNASSIGNED,
/* 01225: ERROR_CONNECTION_REFUSED                                      */ _UNASSIGNED,
/* 01226: ERROR_GRACEFUL_DISCONNECT                                     */ _UNASSIGNED,
/* 01227: ERROR_ADDRESS_ALREADY_ASSOCIATED                              */ _UNASSIGNED,
/* 01228: ERROR_ADDRESS_NOT_ASSOCIATED                                  */ _UNASSIGNED,
/* 01229: ERROR_CONNECTION_INVALID                                      */ _UNASSIGNED,
/* 01230: ERROR_CONNECTION_ACTIVE                                       */ _UNASSIGNED,
/* 01231: ERROR_NETWORK_UNREACHABLE                                     */ _UNASSIGNED,
/* 01232: ERROR_HOST_UNREACHABLE                                        */ _UNASSIGNED,
/* 01233: ERROR_PROTOCOL_UNREACHABLE                                    */ _UNASSIGNED,
/* 01234: ERROR_PORT_UNREACHABLE                                        */ _UNASSIGNED,
/* 01235: ERROR_REQUEST_ABORTED                                         */ _UNASSIGNED,
/* 01236: ERROR_CONNECTION_ABORTED                                      */ _UNASSIGNED,
/* 01237: ERROR_RETRY                                                   */ _UNASSIGNED,
/* 01238: ERROR_CONNECTION_COUNT_LIMIT                                  */ _UNASSIGNED,
/* 01239: ERROR_LOGIN_TIME_RESTRICTION                                  */ _UNASSIGNED,
/* 01240: ERROR_LOGIN_WKSTA_RESTRICTION                                 */ _UNASSIGNED,
/* 01241: ERROR_INCORRECT_ADDRESS                                       */ _UNASSIGNED,
/* 01242: ERROR_ALREADY_REGISTERED                                      */ _UNASSIGNED,
/* 01243: ERROR_SERVICE_NOT_FOUND                                       */ _UNASSIGNED,
/* 01244: ERROR_NOT_AUTHENTICATED                                       */ _UNASSIGNED,
/* 01245: ERROR_NOT_LOGGED_ON                                           */ _UNASSIGNED,
/* 01246: ERROR_CONTINUE                                                */ _UNASSIGNED,
/* 01247: ERROR_ALREADY_INITIALIZED                                     */ _UNASSIGNED,
/* 01248: ERROR_NO_MORE_DEVICES                                         */ _UNASSIGNED,
/* 01249: ERROR_NO_SUCH_SITE                                            */ _UNASSIGNED,
/* 01250: ERROR_DOMAIN_CONTROLLER_EXISTS                                */ _UNASSIGNED,
/* 01251: ERROR_ONLY_IF_CONNECTED                                       */ _UNASSIGNED,
/* 01252: ERROR_OVERRIDE_NOCHANGES                                      */ _UNASSIGNED,
/* 01253: ERROR_BAD_USER_PROFILE                                        */ _UNASSIGNED,
/* 01254: ERROR_NOT_SUPPORTED_ON_SBS                                    */ _UNASSIGNED,
/* 01255: ERROR_SERVER_SHUTDOWN_IN_PROGRESS                             */ _UNASSIGNED,
/* 01256: ERROR_HOST_DOWN                                               */ _UNASSIGNED,
/* 01257: ERROR_NON_ACCOUNT_SID                                         */ _UNASSIGNED,
/* 01258: ERROR_NON_DOMAIN_SID                                          */ _UNASSIGNED,
/* 01259: ERROR_APPHELP_BLOCK                                           */ _UNASSIGNED,
/* 01260: ERROR_ACCESS_DISABLED_BY_POLICY                               */ _UNASSIGNED,
/* 01261: ERROR_REG_NAT_CONSUMPTION                                     */ _UNASSIGNED,
/* 01262: ERROR_CSCSHARE_OFFLINE                                        */ _UNASSIGNED,
/* 01263: ERROR_PKINIT_FAILURE                                          */ _UNASSIGNED,
/* 01264: ERROR_SMARTCARD_SUBSYSTEM_FAILURE                             */ _UNASSIGNED,
/* 01265: ERROR_DOWNGRADE_DETECTED                                      */ _UNASSIGNED,
/* 01266:                                                               */ _UNASSIGNED,
/* 01267:                                                               */ _UNASSIGNED,
/* 01268:                                                               */ _UNASSIGNED,
/* 01269:                                                               */ _UNASSIGNED,
/* 01270:                                                               */ _UNASSIGNED,
/* 01271: ERROR_MACHINE_LOCKED                                          */ _UNASSIGNED,
/* 01272:                                                               */ _UNASSIGNED,
/* 01273: ERROR_CALLBACK_SUPPLIED_INVALID_DATA                          */ _UNASSIGNED,
/* 01274: ERROR_SYNC_FOREGROUND_REFRESH_REQUIRED                        */ _UNASSIGNED,
/* 01275: ERROR_DRIVER_BLOCKED                                          */ _UNASSIGNED,
/* 01276: ERROR_INVALID_IMPORT_OF_NON_DLL                               */ _UNASSIGNED,
/* 01277: ERROR_ACCESS_DISABLED_WEBBLADE                                */ _UNASSIGNED,
/* 01278: ERROR_ACCESS_DISABLED_WEBBLADE_TAMPER                         */ _UNASSIGNED,
/* 01279: ERROR_RECOVERY_FAILURE                                        */ _UNASSIGNED,
/* 01280: ERROR_ALREADY_FIBER                                           */ _UNASSIGNED,
/* 01281: ERROR_ALREADY_THREAD                                          */ _UNASSIGNED,
/* 01282: ERROR_STACK_BUFFER_OVERRUN                                    */ _UNASSIGNED,
/* 01283: ERROR_PARAMETER_QUOTA_EXCEEDED                                */ _UNASSIGNED,
/* 01284: ERROR_DEBUGGER_INACTIVE                                       */ _UNASSIGNED,
/* 01285: ERROR_DELAY_LOAD_FAILED                                       */ _UNASSIGNED,
/* 01286: ERROR_VDM_DISALLOWED                                          */ _UNASSIGNED,
/* 01287: ERROR_UNIDENTIFIED_ERROR                                      */ _UNASSIGNED,
/* 01288: ERROR_INVALID_CRUNTIME_PARAMETER                              */ _UNASSIGNED,
/* 01289: ERROR_BEYOND_VDL                                              */ _UNASSIGNED,
/* 01290: ERROR_INCOMPATIBLE_SERVICE_SID_TYPE                           */ _UNASSIGNED,
/* 01291: ERROR_DRIVER_PROCESS_TERMINATED                               */ _UNASSIGNED,
/* 01292: ERROR_IMPLEMENTATION_LIMIT                                    */ _UNASSIGNED,
/* 01293: ERROR_PROCESS_IS_PROTECTED                                    */ _UNASSIGNED,
/* 01294: ERROR_SERVICE_NOTIFY_CLIENT_LAGGING                           */ _UNASSIGNED,
/* 01295: ERROR_DISK_QUOTA_EXCEEDED                                     */ _UNASSIGNED,
/* 01296: ERROR_CONTENT_BLOCKED                                         */ _UNASSIGNED,
/* 01297: ERROR_INCOMPATIBLE_SERVICE_PRIVILEGE                          */ _UNASSIGNED,
/* 01298: ERROR_APP_HANG                                                */ _UNASSIGNED,
/* 01299: ERROR_INVALID_LABEL                                           */ _UNASSIGNED,
/* 01300: ERROR_NOT_ALL_ASSIGNED                                        */ _UNASSIGNED,
/* 01301: ERROR_SOME_NOT_MAPPED                                         */ _UNASSIGNED,
/* 01302: ERROR_NO_QUOTAS_FOR_ACCOUNT                                   */ _UNASSIGNED,
/* 01303: ERROR_LOCAL_USER_SESSION_KEY                                  */ _UNASSIGNED,
/* 01304: ERROR_NULL_LM_PASSWORD                                        */ _UNASSIGNED,
/* 01305: ERROR_UNKNOWN_REVISION                                        */ _UNASSIGNED,
/* 01306: ERROR_REVISION_MISMATCH                                       */ _UNASSIGNED,
/* 01307: ERROR_INVALID_OWNER                                           */ _UNASSIGNED,
/* 01308: ERROR_INVALID_PRIMARY_GROUP                                   */ _UNASSIGNED,
/* 01309: ERROR_NO_IMPERSONATION_TOKEN                                  */ _UNASSIGNED,
/* 01310: ERROR_CANT_DISABLE_MANDATORY                                  */ _UNASSIGNED,
/* 01311: ERROR_NO_LOGON_SERVERS                                        */ _UNASSIGNED,
/* 01312: ERROR_NO_SUCH_LOGON_SESSION                                   */ _UNASSIGNED,
/* 01313: ERROR_NO_SUCH_PRIVILEGE                                       */ _UNASSIGNED,
/* 01314: ERROR_PRIVILEGE_NOT_HELD                                      */ _UNASSIGNED,
/* 01315: ERROR_INVALID_ACCOUNT_NAME                                    */ _UNASSIGNED,
/* 01316: ERROR_USER_EXISTS                                             */ _UNASSIGNED,
/* 01317: ERROR_NO_SUCH_USER                                            */ _UNASSIGNED,
/* 01318: ERROR_GROUP_EXISTS                                            */ _UNASSIGNED,
/* 01319: ERROR_NO_SUCH_GROUP                                           */ _UNASSIGNED,
/* 01320: ERROR_MEMBER_IN_GROUP                                         */ _UNASSIGNED,
/* 01321: ERROR_MEMBER_NOT_IN_GROUP                                     */ _UNASSIGNED,
/* 01322: ERROR_LAST_ADMIN                                              */ _UNASSIGNED,
/* 01323: ERROR_WRONG_PASSWORD                                          */ _UNASSIGNED,
/* 01324: ERROR_ILL_FORMED_PASSWORD                                     */ _UNASSIGNED,
/* 01325: ERROR_PASSWORD_RESTRICTION                                    */ _UNASSIGNED,
/* 01326: ERROR_LOGON_FAILURE                                           */ _UNASSIGNED,
/* 01327: ERROR_ACCOUNT_RESTRICTION                                     */ _UNASSIGNED,
/* 01328: ERROR_INVALID_LOGON_HOURS                                     */ _UNASSIGNED,
/* 01329: ERROR_INVALID_WORKSTATION                                     */ _UNASSIGNED,
/* 01330: ERROR_PASSWORD_EXPIRED                                        */ _UNASSIGNED,
/* 01331: ERROR_ACCOUNT_DISABLED                                        */ _UNASSIGNED,
/* 01332: ERROR_NONE_MAPPED                                             */ _UNASSIGNED,
/* 01333: ERROR_TOO_MANY_LUIDS_REQUESTED                                */ _UNASSIGNED,
/* 01334: ERROR_LUIDS_EXHAUSTED                                         */ _UNASSIGNED,
/* 01335: ERROR_INVALID_SUB_AUTHORITY                                   */ _UNASSIGNED,
/* 01336: ERROR_INVALID_ACL                                             */ _UNASSIGNED,
/* 01337: ERROR_INVALID_SID                                             */ _UNASSIGNED,
/* 01338: ERROR_INVALID_SECURITY_DESCR                                  */ _UNASSIGNED,
/* 01339:                                                               */ _UNASSIGNED,
/* 01340: ERROR_BAD_INHERITANCE_ACL                                     */ _UNASSIGNED,
/* 01341: ERROR_SERVER_DISABLED                                         */ _UNASSIGNED,
/* 01342: ERROR_SERVER_NOT_DISABLED                                     */ _UNASSIGNED,
/* 01343: ERROR_INVALID_ID_AUTHORITY                                    */ _UNASSIGNED,
/* 01344: ERROR_ALLOTTED_SPACE_EXCEEDED                                 */ _UNASSIGNED,
/* 01345: ERROR_INVALID_GROUP_ATTRIBUTES                                */ _UNASSIGNED,
/* 01346: ERROR_BAD_IMPERSONATION_LEVEL                                 */ _UNASSIGNED,
/* 01347: ERROR_CANT_OPEN_ANONYMOUS                                     */ _UNASSIGNED,
/* 01348: ERROR_BAD_VALIDATION_CLASS                                    */ _UNASSIGNED,
/* 01349: ERROR_BAD_TOKEN_TYPE                                          */ _UNASSIGNED,
/* 01350: ERROR_NO_SECURITY_ON_OBJECT                                   */ _UNASSIGNED,
/* 01351: ERROR_CANT_ACCESS_DOMAIN_INFO                                 */ _UNASSIGNED,
/* 01352: ERROR_INVALID_SERVER_STATE                                    */ _UNASSIGNED,
/* 01353: ERROR_INVALID_DOMAIN_STATE                                    */ _UNASSIGNED,
/* 01354: ERROR_INVALID_DOMAIN_ROLE                                     */ _UNASSIGNED,
/* 01355: ERROR_NO_SUCH_DOMAIN                                          */ _UNASSIGNED,
/* 01356: ERROR_DOMAIN_EXISTS                                           */ _UNASSIGNED,
/* 01357: ERROR_DOMAIN_LIMIT_EXCEEDED                                   */ _UNASSIGNED,
/* 01358: ERROR_INTERNAL_DB_CORRUPTION                                  */ _UNASSIGNED,
/* 01359: ERROR_INTERNAL_ERROR                                          */ _UNASSIGNED,
/* 01360: ERROR_GENERIC_NOT_MAPPED                                      */ _UNASSIGNED,
/* 01361: ERROR_BAD_DESCRIPTOR_FORMAT                                   */ _UNASSIGNED,
/* 01362: ERROR_NOT_LOGON_PROCESS                                       */ _UNASSIGNED,
/* 01363: ERROR_LOGON_SESSION_EXISTS                                    */ _UNASSIGNED,
/* 01364: ERROR_NO_SUCH_PACKAGE                                         */ _UNASSIGNED,
/* 01365: ERROR_BAD_LOGON_SESSION_STATE                                 */ _UNASSIGNED,
/* 01366: ERROR_LOGON_SESSION_COLLISION                                 */ _UNASSIGNED,
/* 01367: ERROR_INVALID_LOGON_TYPE                                      */ _UNASSIGNED,
/* 01368: ERROR_CANNOT_IMPERSONATE                                      */ _UNASSIGNED,
/* 01369: ERROR_RXACT_INVALID_STATE                                     */ _UNASSIGNED,
/* 01370: ERROR_RXACT_COMMIT_FAILURE                                    */ _UNASSIGNED,
/* 01371: ERROR_SPECIAL_ACCOUNT                                         */ _UNASSIGNED,
/* 01372: ERROR_SPECIAL_GROUP                                           */ _UNASSIGNED,
/* 01373: ERROR_SPECIAL_USER                                            */ _UNASSIGNED,
/* 01374: ERROR_MEMBERS_PRIMARY_GROUP                                   */ _UNASSIGNED,
/* 01375: ERROR_TOKEN_ALREADY_IN_USE                                    */ _UNASSIGNED,
/* 01376: ERROR_NO_SUCH_ALIAS                                           */ _UNASSIGNED,
/* 01377: ERROR_MEMBER_NOT_IN_ALIAS                                     */ _UNASSIGNED,
/* 01378: ERROR_MEMBER_IN_ALIAS                                         */ _UNASSIGNED,
/* 01379: ERROR_ALIAS_EXISTS                                            */ _UNASSIGNED,
/* 01380: ERROR_LOGON_NOT_GRANTED                                       */ _UNASSIGNED,
/* 01381: ERROR_TOO_MANY_SECRETS                                        */ _UNASSIGNED,
/* 01382: ERROR_SECRET_TOO_LONG                                         */ _UNASSIGNED,
/* 01383: ERROR_INTERNAL_DB_ERROR                                       */ _UNASSIGNED,
/* 01384: ERROR_TOO_MANY_CONTEXT_IDS                                    */ _UNASSIGNED,
/* 01385: ERROR_LOGON_TYPE_NOT_GRANTED                                  */ _UNASSIGNED,
/* 01386: ERROR_NT_CROSS_ENCRYPTION_REQUIRED                            */ _UNASSIGNED,
/* 01387: ERROR_NO_SUCH_MEMBER                                          */ _UNASSIGNED,
/* 01388: ERROR_INVALID_MEMBER                                          */ _UNASSIGNED,
/* 01389: ERROR_TOO_MANY_SIDS                                           */ _UNASSIGNED,
/* 01390: ERROR_LM_CROSS_ENCRYPTION_REQUIRED                            */ _UNASSIGNED,
/* 01391: ERROR_NO_INHERITANCE                                          */ _UNASSIGNED,
/* 01392: ERROR_FILE_CORRUPT                                            */ _UNASSIGNED,
/* 01393: ERROR_DISK_CORRUPT                                            */ _UNASSIGNED,
/* 01394: ERROR_NO_USER_SESSION_KEY                                     */ _UNASSIGNED,
/* 01395: ERROR_LICENSE_QUOTA_EXCEEDED                                  */ _UNASSIGNED,
/* 01396: ERROR_WRONG_TARGET_NAME                                       */ _UNASSIGNED,
/* 01397: ERROR_MUTUAL_AUTH_FAILED                                      */ _UNASSIGNED,
/* 01398: ERROR_TIME_SKEW                                               */ _UNASSIGNED,
/* 01399: ERROR_CURRENT_DOMAIN_NOT_ALLOWED                              */ _UNASSIGNED,
/* 01400: ERROR_INVALID_WINDOW_HANDLE                                   */ _UNASSIGNED,
/* 01401: ERROR_INVALID_MENU_HANDLE                                     */ _UNASSIGNED,
/* 01402: ERROR_INVALID_CURSOR_HANDLE                                   */ _UNASSIGNED,
/* 01403: ERROR_INVALID_ACCEL_HANDLE                                    */ _UNASSIGNED,
/* 01404: ERROR_INVALID_HOOK_HANDLE                                     */ _UNASSIGNED,
/* 01405: ERROR_INVALID_DWP_HANDLE                                      */ _UNASSIGNED,
/* 01406: ERROR_TLW_WITH_WSCHILD                                        */ _UNASSIGNED,
/* 01407: ERROR_CANNOT_FIND_WND_CLASS                                   */ _UNASSIGNED,
/* 01408: ERROR_WINDOW_OF_OTHER_THREAD                                  */ _UNASSIGNED,
/* 01409: ERROR_HOTKEY_ALREADY_REGISTERED                               */ _UNASSIGNED,
/* 01410: ERROR_CLASS_ALREADY_EXISTS                                    */ _UNASSIGNED,
/* 01411: ERROR_CLASS_DOES_NOT_EXIST                                    */ _UNASSIGNED,
/* 01412: ERROR_CLASS_HAS_WINDOWS                                       */ _UNASSIGNED,
/* 01413: ERROR_INVALID_INDEX                                           */ _UNASSIGNED,
/* 01414: ERROR_INVALID_ICON_HANDLE                                     */ _UNASSIGNED,
/* 01415: ERROR_PRIVATE_DIALOG_INDEX                                    */ _UNASSIGNED,
/* 01416: ERROR_LISTBOX_ID_NOT_FOUND                                    */ _UNASSIGNED,
/* 01417: ERROR_NO_WILDCARD_CHARACTERS                                  */ _UNASSIGNED,
/* 01418: ERROR_CLIPBOARD_NOT_OPEN                                      */ _UNASSIGNED,
/* 01419: ERROR_HOTKEY_NOT_REGISTERED                                   */ _UNASSIGNED,
/* 01420: ERROR_WINDOW_NOT_DIALOG                                       */ _UNASSIGNED,
/* 01421: ERROR_CONTROL_ID_NOT_FOUND                                    */ _UNASSIGNED,
/* 01422: ERROR_INVALID_COMBOBOX_MESSAGE                                */ _UNASSIGNED,
/* 01423: ERROR_WINDOW_NOT_COMBOBOX                                     */ _UNASSIGNED,
/* 01424: ERROR_INVALID_EDIT_HEIGHT                                     */ _UNASSIGNED,
/* 01425: ERROR_DC_NOT_FOUND                                            */ _UNASSIGNED,
/* 01426: ERROR_INVALID_HOOK_FILTER                                     */ _UNASSIGNED,
/* 01427: ERROR_INVALID_FILTER_PROC                                     */ _UNASSIGNED,
/* 01428: ERROR_HOOK_NEEDS_HMOD                                         */ _UNASSIGNED,
/* 01429: ERROR_GLOBAL_ONLY_HOOK                                        */ _UNASSIGNED,
/* 01430: ERROR_JOURNAL_HOOK_SET                                        */ _UNASSIGNED,
/* 01431: ERROR_HOOK_NOT_INSTALLED                                      */ _UNASSIGNED,
/* 01432: ERROR_INVALID_LB_MESSAGE                                      */ _UNASSIGNED,
/* 01433: ERROR_SETCOUNT_ON_BAD_LB                                      */ _UNASSIGNED,
/* 01434: ERROR_LB_WITHOUT_TABSTOPS                                     */ _UNASSIGNED,
/* 01435: ERROR_DESTROY_OBJECT_OF_OTHER_THREAD                          */ _UNASSIGNED,
/* 01436: ERROR_CHILD_WINDOW_MENU                                       */ _UNASSIGNED,
/* 01437: ERROR_NO_SYSTEM_MENU                                          */ _UNASSIGNED,
/* 01438: ERROR_INVALID_MSGBOX_STYLE                                    */ _UNASSIGNED,
/* 01439: ERROR_INVALID_SPI_VALUE                                       */ _UNASSIGNED,
/* 01440: ERROR_SCREEN_ALREADY_LOCKED                                   */ _UNASSIGNED,
/* 01441: ERROR_HWNDS_HAVE_DIFF_PARENT                                  */ _UNASSIGNED,
/* 01442: ERROR_NOT_CHILD_WINDOW                                        */ _UNASSIGNED,
/* 01443: ERROR_INVALID_GW_COMMAND                                      */ _UNASSIGNED,
/* 01444: ERROR_INVALID_THREAD_ID                                       */ _UNASSIGNED,
/* 01445: ERROR_NON_MDICHILD_WINDOW                                     */ _UNASSIGNED,
/* 01446: ERROR_POPUP_ALREADY_ACTIVE                                    */ _UNASSIGNED,
/* 01447: ERROR_NO_SCROLLBARS                                           */ _UNASSIGNED,
/* 01448: ERROR_INVALID_SCROLLBAR_RANGE                                 */ _UNASSIGNED,
/* 01449: ERROR_INVALID_SHOWWIN_COMMAND                                 */ _UNASSIGNED,
/* 01450: ERROR_NO_SYSTEM_RESOURCES                                     */ _UNASSIGNED,
/* 01451: ERROR_NONPAGED_SYSTEM_RESOURCES                               */ _UNASSIGNED,
/* 01452: ERROR_PAGED_SYSTEM_RESOURCES                                  */ _UNASSIGNED,
/* 01453: ERROR_WORKING_SET_QUOTA                                       */ _UNASSIGNED,
/* 01454: ERROR_PAGEFILE_QUOTA                                          */ _UNASSIGNED,
/* 01455: ERROR_COMMITMENT_LIMIT                                        */ _UNASSIGNED,
/* 01456: ERROR_MENU_ITEM_NOT_FOUND                                     */ _UNASSIGNED,
/* 01457: ERROR_INVALID_KEYBOARD_HANDLE                                 */ _UNASSIGNED,
/* 01458: ERROR_HOOK_TYPE_NOT_ALLOWED                                   */ _UNASSIGNED,
/* 01459: ERROR_REQUIRES_INTERACTIVE_WINDOWSTATION                      */ _UNASSIGNED,
/* 01460: ERROR_TIMEOUT                                                 */ _UNASSIGNED,
/* 01461: ERROR_INVALID_MONITOR_HANDLE                                  */ _UNASSIGNED,
/* 01462: ERROR_INCORRECT_SIZE                                          */ _UNASSIGNED,
/* 01463: ERROR_SYMLINK_CLASS_DISABLED                                  */ _UNASSIGNED,
/* 01464: ERROR_SYMLINK_NOT_SUPPORTED                                   */ _UNASSIGNED,
/* 01465: ERROR_XML_PARSE_ERROR                                         */ _UNASSIGNED,
/* 01466: ERROR_XMLDSIG_ERROR                                           */ _UNASSIGNED,
/* 01467: ERROR_RESTART_APPLICATION                                     */ _UNASSIGNED,
/* 01468: ERROR_WRONG_COMPARTMENT                                       */ _UNASSIGNED,
/* 01469: ERROR_AUTHIP_FAILURE                                          */ _UNASSIGNED,
/* 01470: ERROR_NO_NVRAM_RESOURCES                                      */ _UNASSIGNED
};

static const uint16_t WinError_table_1550_to_1552[] =
{
/* 01550: ERROR_INVALID_TASK_NAME                                       */ _UNASSIGNED,
/* 01551: ERROR_INVALID_TASK_INDEX                                      */ _UNASSIGNED,
/* 01552: ERROR_THREAD_ALREADY_IN_TASK                                  */ _UNASSIGNED
};

static const uint16_t WinError_table_2000_to_2023[] =
{
/* 02000: ERROR_INVALID_PIXEL_FORMAT                                    */ _MAP(ERR_RT_INVALID_ARGUMENT),
/* 02001: ERROR_BAD_DRIVER                                              */ _UNASSIGNED,
/* 02002: ERROR_INVALID_WINDOW_STYLE                                    */ _UNASSIGNED,
/* 02003: ERROR_METAFILE_NOT_SUPPORTED                                  */ _UNASSIGNED,
/* 02004: ERROR_TRANSFORM_NOT_SUPPORTED                                 */ _UNASSIGNED,
/* 02005: ERROR_CLIPPING_NOT_SUPPORTED                                  */ _UNASSIGNED,
/* 02006:                                                               */ _UNASSIGNED,
/* 02007:                                                               */ _UNASSIGNED,
/* 02008:                                                               */ _UNASSIGNED,
/* 02009:                                                               */ _UNASSIGNED,
/* 02010: ERROR_INVALID_CMM                                             */ _UNASSIGNED,
/* 02011: ERROR_INVALID_PROFILE                                         */ _UNASSIGNED,
/* 02012: ERROR_TAG_NOT_FOUND                                           */ _UNASSIGNED,
/* 02013: ERROR_TAG_NOT_PRESENT                                         */ _UNASSIGNED,
/* 02014: ERROR_DUPLICATE_TAG                                           */ _UNASSIGNED,
/* 02015: ERROR_PROFILE_NOT_ASSOCIATED_WITH_DEVICE                      */ _UNASSIGNED,
/* 02016: ERROR_PROFILE_NOT_FOUND                                       */ _UNASSIGNED,
/* 02017: ERROR_INVALID_COLORSPACE                                      */ _UNASSIGNED,
/* 02018: ERROR_ICM_NOT_ENABLED                                         */ _UNASSIGNED,
/* 02019: ERROR_DELETING_ICM_XFORM                                      */ _UNASSIGNED,
/* 02020: ERROR_INVALID_TRANSFORM                                       */ _UNASSIGNED,
/* 02021: ERROR_COLORSPACE_MISMATCH                                     */ _UNASSIGNED,
/* 02022: ERROR_INVALID_COLORINDEX                                      */ _UNASSIGNED,
/* 02023: ERROR_PROFILE_DOES_NOT_MATCH_DEVICE                           */ _UNASSIGNED
};

// ============================================================================
// [Fog::WinUtil - Error Translation]
// ============================================================================

#define _MAP_RANGE(_Start_, _End_) \
  FOG_MACRO_BEGIN \
    if (code >= _Start_ && code <= _End_) \
    { \
      map = WinError_table_##_Start_##_to_##_End_[code - _Start_]; \
      goto _Map; \
    } \
  FOG_MACRO_END

static err_t FOG_CDECL WinUtil_getErrFromWinErrorCode(DWORD code)
{
  uint32_t map;

  if (code == 0)
    return ERR_OK;

  _MAP_RANGE(0, 353);
  _MAP_RANGE(400, 403);
  _MAP_RANGE(534, 805);
  _MAP_RANGE(994, 1470);
  _MAP_RANGE(1550, 1552);
  _MAP_RANGE(2000, 2023);

  return code;

_Map:
  if (map != 0xFFFF)
    return map + _ERR_RANGE_FIRST;
  else
    return code;
}

static err_t FOG_CDECL WinUtil_getErrFromWinLastError(void)
{
  return WinUtil::getErrFromWinErrorCode(::GetLastError());
}

// ============================================================================
// [Fog::WinUtil - Helpers (Windows)]
// ============================================================================

static WinVersion WinUtil_winVersion;

static void WinUtil_startup()
{
  OSVERSIONINFOEXW os;

  ZeroMemory(&os, sizeof(OSVERSIONINFOEXW));
  os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
  GetVersionExW((OSVERSIONINFOW*)&os);

  WinUtil_winVersion.version = WIN_VERSION_UNKNOWN;
  WinUtil_winVersion.major = os.dwMajorVersion;
  WinUtil_winVersion.minor = os.dwMinorVersion;
  WinUtil_winVersion.build = os.dwBuildNumber;

  switch (os.dwPlatformId)
  {
    case VER_PLATFORM_WIN32_NT:
      WinUtil_winVersion.version = WIN_VERSION_NT;

      switch (os.dwMajorVersion)
      {
        case 0:
        case 1:
        case 2:
          break;

        // Windows NT 3.0
        case 3:
          WinUtil_winVersion.version = WIN_VERSION_NT_3;
          break;

        // Windows NT 4.0
        case 4:
          WinUtil_winVersion.version = WIN_VERSION_NT_4;
          break;

        // Windows 2000, XP or 2003
        case 5:
          WinUtil_winVersion.version = WIN_VERSION_2000;

          if (os.dwMinorVersion == 1)
          {
            WinUtil_winVersion.version = WIN_VERSION_XP;
          }

          if (os.dwMinorVersion == 2)
          {
            WinUtil_winVersion.version = WIN_VERSION_2003;

            if (::GetSystemMetrics(SM_SERVERR2) != 0)
              WinUtil_winVersion.version = WIN_VERSION_2003_R2;
          }
          break;

        // Windows Vista, Windows 7, Windows 2008.
        case 6:
          if (os.wProductType == VER_NT_WORKSTATION)
          {
            WinUtil_winVersion.version = WIN_VERSION_VISTA;

            if (os.dwMinorVersion == 1)
              WinUtil_winVersion.version = WIN_VERSION_7;

            if (os.dwMinorVersion > 1)
              goto _Future;
          }
          else
          {
            WinUtil_winVersion.version = WIN_VERSION_2008;

            if (os.dwMinorVersion == 1)
              WinUtil_winVersion.version = WIN_VERSION_2008_R2;

            if (os.dwMinorVersion > 1)
              goto _Future;
          }
          break;

        default:
_Future:
          WinUtil_winVersion.version = WIN_VERSION_FUTURE;
          break;
      }
      break;
  }
}

// ============================================================================
// [Fog::WinUtil - Info (Windows)]
// ============================================================================

static uint32_t FOG_CDECL WinUtil_getWinVersion(WinVersion* dst)
{
  if (dst != NULL)
    MemOps::copy(dst, &WinUtil_winVersion, sizeof(WinVersion));

  return WinUtil_winVersion.version;
}

static err_t FOG_CDECL WinUtil_getWinDirectory(StringW* dst)
{
  CharW* buf = dst->_prepare(CONTAINER_OP_REPLACE, PATH_MAX);
  if (FOG_IS_NULL(buf))
    return ERR_RT_OUT_OF_MEMORY;

  UINT result = ::GetWindowsDirectoryW(reinterpret_cast<wchar_t*>(buf), PATH_MAX);
  if (result == 0 || result >= PATH_MAX)
    return dst->set(Ascii8("C:/Windows"));

  dst->_modified(buf + result);
  return dst->normalizeSlashes(SLASH_FORM_UNIX);
}

static err_t FOG_CDECL WinUtil_getModuleFileName(StringW* dst, HMODULE hModule)
{
  size_t capacity = 128;

  for (;;)
  {
    CharW* buf = dst->_prepare(CONTAINER_OP_REPLACE, capacity);
    capacity = dst->getCapacity();

    if (FOG_IS_NULL(buf))
      return ERR_RT_OUT_OF_MEMORY;

    size_t length = ::GetModuleFileNameW(hModule,
      reinterpret_cast<wchar_t*>(buf),
      static_cast<DWORD>(Math::min<size_t>(capacity + 1, UINT32_MAX)));

    if (length == 0)
      return WinUtil::getErrFromWinLastError();

    if (length >= capacity)
    {
      capacity *= 2;
      continue;
    }

    dst->_modified(buf + length);
    return ERR_OK;
  }
}

// TODO: Rewrite, not optimal, toAbsolute() should be modified to append
// the path into dst instead of replace it (use CONTAINER_OP_...).

static err_t FOG_CDECL WinUtil_makeWinPathStubW(StringW* dst, const StubW* src)
{
  FOG_RETURN_ON_ERROR(FilePath::toAbsolute(*dst, *src));
  FOG_RETURN_ON_ERROR(dst->normalizeSlashes(SLASH_FORM_WINDOWS));

  // Do not translate the root path like C:\ to \\?\C:\, because Win-API do not
  // expect that!
  if (dst->getLength() > 3)
    FOG_RETURN_ON_ERROR(dst->prepend(Ascii8("\\\\?\\")));

  return ERR_OK;
}

static err_t FOG_CDECL WinUtil_makeWinPathStringW(StringW* dst, const StringW* src)
{
  FOG_RETURN_ON_ERROR(FilePath::toAbsolute(*dst, *src));
  FOG_RETURN_ON_ERROR(dst->normalizeSlashes(SLASH_FORM_WINDOWS));

  // Do not translate the root path like C:\ to \\?\C:\, because Win-API do not
  // expect that!
  if (dst->getLength() > 3)
    FOG_RETURN_ON_ERROR(dst->prepend(Ascii8("\\\\?\\")));

  return ERR_OK;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void WinUtil_Core_init(void)
{
  WinUtil_startup();

  fog_api.winutil_getErrFromWinErrorCode = WinUtil_getErrFromWinErrorCode;
  fog_api.winutil_getErrFromWinLastError = WinUtil_getErrFromWinLastError;

  fog_api.winutil_getWinVersion = WinUtil_getWinVersion;
  fog_api.winutil_getWinDirectory = WinUtil_getWinDirectory;

  fog_api.winutil_getModuleFileName = WinUtil_getModuleFileName;

  fog_api.winutil_makeWinPathStubW = WinUtil_makeWinPathStubW;
  fog_api.winutil_makeWinPathStringW = WinUtil_makeWinPathStringW;
}

} // Fog namespace
