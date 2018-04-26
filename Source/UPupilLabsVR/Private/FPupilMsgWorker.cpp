// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Chifor Tudor

#include "FPupilMsgWorker.h"

FPupilMsgWorker* FPupilMsgWorker::Instance = nullptr;

/**
 * \brief Thread which connects and receive data from a certain @PupilTopic from Pupil Serice.
 * Start- and StopListening are all what is needed to use this Thread.
 */
FPupilMsgWorker::FPupilMsgWorker()
{
	Thread = FRunnableThread::Create(this, TEXT("PupilMsgWorker"), TPri_BelowNormal);
}

/**
 * \brief Deconstructor that waits for completion of the Task and then deletes the Thread.
 */
FPupilMsgWorker::~FPupilMsgWorker()
{
	if (Thread != nullptr) {
		Thread->WaitForCompletion();
		delete Thread;
		Thread = nullptr;
	}
}

/**
 * \brief Initializes the context and socket to connect to Pupil Service.
 *	Starts Run() if successful.
 * \return true if the Initialization was  successful
 */
bool FPupilMsgWorker::Init()
{	
	 PupilHelper = FPupilLabsUtils();
	
	return true; //TODO BUILD A ERROR BASED LOG. FALSE IF THE INITIALISATION FAILED
}

/**
* \brief Constantly retrives data from Pupil Service if it has been successfuly initialized & nobody has stopped the Thread.
* \return A number indicating where it has failed TODO
*/
uint32 FPupilMsgWorker::Run()
{
	FPlatformProcess::Sleep(0.03);

	while (bRunning && bSuccessfulyInit)
	{
		FPlatformProcess::Sleep(0.32);

		float DummyEllipseData;

		DummyEllipseData = PupilHelper.GetDummyElipseData();
		UE_LOG(LogTemp, Warning, TEXT("ZMQ>>>>Subport : %f"), DummyEllipseData);

	}
	return 1;
}

void FPupilMsgWorker::Stop()
{
	Instance->bRunning = false;
}

void FPupilMsgWorker::EnsureCompletion()
{
	Stop();
	Thread->WaitForCompletion();
}

void FPupilMsgWorker::Shutdown()
{
	if (Instance)
	{
		Instance->EnsureCompletion();
		delete Instance;
		Instance = nullptr;
	}
}

/**
* \brief This method checks if possible and then starts a Thread and Initializes it.
* The Initialization also starts the Thread by calling the Run() method.
* \return Instance of the object containing the Worker Thread
*/
FPupilMsgWorker* FPupilMsgWorker::StartListening()
{
	//Create new instance of thread if it does not exist
	//		and the platform supports multi threading!
	if (!Instance && FPlatformProcess::SupportsMultithreading())
	{
		Instance = new FPupilMsgWorker();
		Instance->bSuccessfulyInit = Instance->Init();
		Instance->bRunning = true;
	}
	return Instance;
}

/**
 * \brief Waits and Stops the Thread in the correct way.Stops the process and then destroys the Thread.
 */
void FPupilMsgWorker::StopListening()
{
	if (Thread != nullptr) {
		this->Stop();
		Thread->WaitForCompletion();
		delete Thread;
		Thread = nullptr;
	}
}