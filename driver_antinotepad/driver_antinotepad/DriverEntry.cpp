
// Requires /INTEGRITYCHECK in the linker flags. Project Properties -> Linker -> Command Line -> Additional Options (at the bottom)

// Relevant: https://stackoverflow.com/questions/51853537/error-lnk2019-unresolved-external-symbol-checkfordebuggerjustmycode-reference/51866913

// Another ObCallback group setup so we can check for changed handle privileges.

// Some more notify stuff:
//   PsSetCreateThreadNotifyRoutine
//   PsSetCreateThreadNotifyRoutineEx

//   PsSetLoadImageNotifyRoutine
//   PsSetLoadImageNotifyRoutineEx
//     These can be checked for Process Doppelganging most likely.
//     Aka: Check the FileObject of the loaded image (doppelganging dlls maybe).

// Pass off information from notify routines to system worker threads
// https://docs.microsoft.com/en-us/windows-hardware/drivers/kernel/system-worker-threads

// IoRegisterShutdownNotification 


#include <ntddk.h>
#include <string.h>
//#include "../../UNICODE_STRING_utils.h"
#include <aa/UNICODE_STRING_utils.h>

#define MyPrint(s, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, s, __VA_ARGS__)

EXTERN_C DRIVER_INITIALIZE DriverEntry;
EXTERN_C DRIVER_UNLOAD DriverUnload;

DECLARE_CONST_UNICODE_STRING(g_NotepadImageName, L"notepad.exe");

// https://docs.microsoft.com/en-us/windows-hardware/drivers/ifs/load-order-groups-and-altitudes-for-minifilter-drivers
// Figure out what the fuck the altitudes are about.
DECLARE_CONST_UNICODE_STRING(g_CallbackAltitude, L"999");

static PVOID g_ObCallbackRegistrationHandle = NULL;
static BOOLEAN g_CreateProcessNotifyRegisterd = FALSE;


OB_PREOP_CALLBACK_STATUS PobPreOperationCallback_Process(
	PVOID RegistrationContext,
	POB_PRE_OPERATION_INFORMATION OperationInformation
)
{
	UNREFERENCED_PARAMETER(RegistrationContext);
	UNREFERENCED_PARAMETER(OperationInformation);
	return OB_PREOP_SUCCESS;
}

OB_PREOP_CALLBACK_STATUS PobPreOperationCallback_Thread(
	PVOID RegistrationContext,
	POB_PRE_OPERATION_INFORMATION OperationInformation
)
{
	UNREFERENCED_PARAMETER(RegistrationContext);
	UNREFERENCED_PARAMETER(OperationInformation);
	return OB_PREOP_SUCCESS;
}

OB_PREOP_CALLBACK_STATUS PobPreOperationCallback_DesktopObject(
	PVOID RegistrationContext,
	POB_PRE_OPERATION_INFORMATION OperationInformation
)
{
	UNREFERENCED_PARAMETER(RegistrationContext);
	UNREFERENCED_PARAMETER(OperationInformation);
	return OB_PREOP_SUCCESS;
}

void PobPostOperationCallback_Process(
	PVOID RegistrationContext,
	POB_POST_OPERATION_INFORMATION OperationInformation
)
{
	UNREFERENCED_PARAMETER(RegistrationContext);
	UNREFERENCED_PARAMETER(OperationInformation);
}

void PobPostOperationCallback_Thread(
	PVOID RegistrationContext,
	POB_POST_OPERATION_INFORMATION OperationInformation
)
{
	UNREFERENCED_PARAMETER(RegistrationContext);
	UNREFERENCED_PARAMETER(OperationInformation);
}

void PobPostOperationCallback_DesktopObject(
	PVOID RegistrationContext,
	POB_POST_OPERATION_INFORMATION OperationInformation
)
{
	UNREFERENCED_PARAMETER(RegistrationContext);
	UNREFERENCED_PARAMETER(OperationInformation);
}
NTSTATUS SetupCallbacks(/*OB_CALLBACK_REGISTRATION * registration,*/ PVOID * registrationHandle)
{
	static const OB_OPERATION_REGISTRATION g_ObOperationRegistration[] = {
	{
		PsProcessType, // ObjectType
		OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE, // Operations
		PobPreOperationCallback_Process, // PreOperation
		PobPostOperationCallback_Process // PostOperation
	},
	{
		PsThreadType, // ObjectType
		OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE, // Operations
		PobPreOperationCallback_Thread, // PreOperation
		PobPostOperationCallback_Thread // PostOperation
	},
	{
		ExDesktopObjectType, // ObjectType
		OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE, // Operations
		PobPreOperationCallback_DesktopObject, // PreOperation
		PobPostOperationCallback_DesktopObject // PostOperation
	},
	};

	static const OB_CALLBACK_REGISTRATION g_ObCallbackRegistration = {
		OB_FLT_REGISTRATION_VERSION, // Version
		sizeof(g_ObOperationRegistration) / sizeof(g_ObOperationRegistration[0]), // OperationRegistrationCount
		g_CallbackAltitude, // Altitude
		NULL, // OperationRegistration
		(OB_OPERATION_REGISTRATION *)g_ObOperationRegistration // OperationRegistration
	};

	NTSTATUS status;

	status = ObRegisterCallbacks(
		(OB_CALLBACK_REGISTRATION *)&g_ObCallbackRegistration,//registration,
		registrationHandle
	);

	switch (status) {
	case STATUS_SUCCESS:
		MyPrint("[antinotepad] Successfully registered ObCallbacks");
		break;
	case STATUS_FLT_INSTANCE_ALTITUDE_COLLISION:
		MyPrint("[antinotepad] ObRegisterCallbacks() = STATUS_FLT_INSTANCE_ALTITUDE_COLLISION");
		MyPrint("[antinotepad]   Who the fuck stole our altitude?");
		break;
	case STATUS_INVALID_PARAMETER:
		MyPrint("[antinotepad] ObRegisterCallbacks() = STATUS_INVALID_PARAMETER");
		break;
	case STATUS_ACCESS_DENIED:
		MyPrint("[antinotepad] ObRegisterCallbacks() = STATUS_ACCESS_DENIED --- sign your driver");
		break;
	case STATUS_INSUFFICIENT_RESOURCES:
		MyPrint("[antinotepad] ObRegisterCallbacks() = STATUS_INSUFFICIENT_RESOURCES");
		break;
	};

	return status;
}

void PcreateProcessNotifyRoutineEx(
	PEPROCESS Process,
	HANDLE ProcessId,
	PPS_CREATE_NOTIFY_INFO CreateInfo
)
{
	UNREFERENCED_PARAMETER(Process);
	UNREFERENCED_PARAMETER(ProcessId);

	PCUNICODE_STRING ImageFileName;
	UNICODE_STRING usBaseFileName;

	if (!CreateInfo) {
		// The process is exiting.
		return;
	}

	if (CreateInfo->IsSubsystemProcess) {
		// WSL process.
		return;
	}

	if (NULL == (ImageFileName = CreateInfo->ImageFileName)) {
		// That'd be wrong...
		CreateInfo->CreationStatus = STATUS_ACCESS_DENIED;
		return;
	}

	// Block attempted Process Doppelganging
	if (CreateInfo->FileObject->WriteAccess) {
		CreateInfo->CreationStatus = STATUS_ACCESS_DENIED;
		return;
	}

	//MyPrint("ImageFileName = `%wZ`", ImageFileName);

	if (FALSE == AA_usBaseFile(ImageFileName, &usBaseFileName)) {
		MyPrint("Couldn't get the base file name... should I block creation?");
		return;
	}

	// Can print out inaccurate data due the ImageFileName (and thus usBaseFileName) being stale.
	//MyPrint("usBaseFileName = `%wZ`", usBaseFileName);

	// Case-insensitive compare.
	if (TRUE == RtlEqualUnicodeString(&g_NotepadImageName, &usBaseFileName, TRUE)) {
		CreateInfo->CreationStatus = STATUS_ACCESS_DENIED;
		MyPrint("Blocking %wZ", &g_NotepadImageName);
		return;
	}
}

EXTERN_C NTSTATUS DriverEntry(
	PDRIVER_OBJECT DriverObject,
	PUNICODE_STRING RegstiryPath
)
{
	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(RegstiryPath);

	NTSTATUS status = STATUS_SUCCESS;

	DriverObject->DriverUnload = DriverUnload;

	MyPrint("This is a test string `%wZ`", &g_NotepadImageName);

	status = PsSetCreateProcessNotifyRoutineEx(PcreateProcessNotifyRoutineEx, FALSE);
	switch (status)
	{
	case STATUS_SUCCESS:
		// Nice
		break;
	case STATUS_INVALID_PARAMETER:
		// The specified routine was already registered,
		// or the operating system has reached its limit for
		// registering process-creation callback routines.
		break;
	case STATUS_ACCESS_DENIED:
		// Missing IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY... need /integritycheck
		break;
	}
	if (!NT_SUCCESS(status))
		goto out;

	status = SetupCallbacks(
		//(OB_CALLBACK_REGISTRATION *)&g_ObCallbackRegistration,
		&g_ObCallbackRegistrationHandle
	);

	// Setup two callback chains so we check for handle changes and revert them.
	//
	//status = SetupCallbacks(
	//	(OB_CALLBACK_REGISTRATION *)&g_ObCallbackRegistration_High,
	//	&g_ObCallbackRegistrationHandle_High
	//);

out:
	return status;
	//return STATUS_SUCCESS;
}

EXTERN_C void DriverUnload(
	PDRIVER_OBJECT DriverObject
)
{
	UNREFERENCED_PARAMETER(DriverObject);

	NTSTATUS status = STATUS_SUCCESS;

	MyPrint("Bye bye");

	// Remove our CreateProcess notify routine.
	if (g_CreateProcessNotifyRegisterd) {
		status = PsSetCreateProcessNotifyRoutineEx(PcreateProcessNotifyRoutineEx, TRUE);
		(void)status;
		g_CreateProcessNotifyRegisterd = FALSE;
	}

	// Unregister callbacks.
	if (g_ObCallbackRegistrationHandle) {
		ObUnRegisterCallbacks(g_ObCallbackRegistrationHandle);
		g_ObCallbackRegistrationHandle = NULL;
	}
}
