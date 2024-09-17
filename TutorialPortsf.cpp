// TutorialPortsf.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <portsf.h>
#include <stdio.h>
const char* ConvertToChannelFormat(psf_channelformat channelFormat) 
{
    /*STDWAVE,MC_STD,MC_MONO,MC_STEREO,MC_QUAD,MC_LCRS,MC_BFMT,MC_DOLBY_5_1,MC_SURR_5_0,MC_SURR_7_1,MC_WAVE_EX */
    switch (channelFormat)
    {
        case 0:
            return "STDWAVE";
            break;
        case 1:
            return "MC_STD";
            break;
        case 2:
            return "MC_MONO";
            break;
        case 3:
            return "MC_STEREO";
            break;
        case 4:
            return "MC_LCRS";
            break;
        case 5:
            return "MC_BFMT";
            break;
        case 6:
            return "MC_DOLBY_5_1";
            break;
        case 7:
            return "MC_SURR_5_0";
            break;
        case 8:
            return "MC_SURR_7_1";
            break;
        case 9:
            return "MC_WAVE_EX";
            break;
        default:
            return "ERROR!";
            break;
    }
}

const char* ConvertToFormat(psf_format format) 
{
    /*
    typedef enum {
	PSF_FMT_UNKNOWN = 0,		 e.g if no extension given. This could also signify 'raw' 
    PSF_STDWAVE,
       PSF_WAVE_EX,
        PSF_AIFF,
        PSF_AIFC
} psf_format;
    */

    switch (format)
    {
    case 0:
        return "PSF_FMT_UNKNOWN";
    case 1:
        return "PSF_STDWAVE";
    case 2:
        return "PSF_WAVE_EX";
    case 3:
        return "PSF_AIFF";
    case 4:
        return "PSF_AIFC";
    default:
        return "ERROR!";
        break;
    }

}

const char* ConvertToSampleType(psf_stype sampleType)
{
    /*
    *   PSF_SAMP_UNKNOWN	=	0,
	    PSF_SAMP_8,				   
        PSF_SAMP_16,
        PSF_SAMP_24,
        PSF_SAMP_32,
        PSF_SAMP_IEEE_FLOAT
    */
    switch (sampleType)
    {
    case 0:
        return "PSF_SAMP_UNKNOWN";
    case 1:
        return "PSF_SAMP_8";
    case 2:
        return "PSF_SAMP_16";
    case 3:
        return "PSF_SAMP_24";
    case 4:
        return "PSF_SAMP_32";
    case 5:
        return "PSF_SAMP_IEEE_FLOAT";
    default:
        return "ERROR!";
        break;
    }
}


int main()
{
    psf_props props;
    int inputSoundfile = -1;
    int outputSoundfile = -1;
    int error = 0;
    const char* inputFilePath = "./assets/Audio_01.wav";
    const char* outputFilePath = "./assets/Audio_02.wav";
    psf_format outputFormat = PSF_FMT_UNKNOWN;
    float* frame = NULL;
    PSF_CHPEAK* peaks = NULL;
    int framesread, totalread;
    int bufferSize = 2048;
    int totalFramesInputFile;

    //Inicializamos portsf
    if (psf_init())
    {
        printf("Unable to start");
        return -1;
    }

    //Abrimos el archivo de entrada
    inputSoundfile = psf_sndOpen(inputFilePath, &props, 0);

    if (inputSoundfile < 0)
    {
        printf("Error opening the input soundfile with the path %s\n", inputFilePath);
        return -1;
    }

    //Checamos si el archivo de entrada ya tiene el formato de floats en vez de pcm
    if (props.samptype == PSF_SAMP_IEEE_FLOAT)
    {
        printf("Info: The input soundfile is already in floats format");
    }

    printf("Properties of the input soundfile: \n");
    printf("\t-Sample Type: %s \n",ConvertToSampleType(props.samptype));
    printf("\t-Channels: %d \n", props.chans);
    printf("\t-Sample Rate: %d \n", props.srate);
    printf("\t-Format: %s \n", ConvertToFormat(props.format));
    printf("\t-Channel Format: %s \n", ConvertToChannelFormat(props.chformat));
    printf("\t-Total Frames: %d \n", psf_sndSize(inputSoundfile));

    //Hacemos su samptype que sea floats
    props.samptype = PSF_SAMP_IEEE_FLOAT;

    //guardamos el formato del archivo
    outputFormat = psf_getFormatExt(inputFilePath);
    if (outputFormat == PSF_FMT_UNKNOWN)
    {
        printf("Outputfile with the path %s has unknown format.\n", outputFilePath);
        error++;
        goto exit;
    }
    props.format = outputFormat;
    
    //Creamos asignamos un valor al output soundfile
    outputSoundfile = psf_sndCreate(outputFilePath, &props, 0,0, PSF_CREATE_RDWR);
    if (outputSoundfile < 0)
    {
        printf("Error creating the sound file data of the file in the path %s", outputFilePath);
        error++;
        goto exit;
    }

    //Destinamos espaico en memoria para el buffer también llamado frame

    frame = (float*) malloc(bufferSize*props.chans * sizeof(float));
    if (frame == NULL)
    {
        puts("No memmory!\n");
        error++;
        goto exit;
    }

    //Memoria para los peaks que no sabremos durante compile time
    peaks = (PSF_CHPEAK*)malloc(props.chans * sizeof(PSF_CHPEAK));
    if (peaks == NULL)
    {
        puts("No memory");
        error++;
        goto exit;
    }

    printf("copying....\n");

    totalFramesInputFile = psf_sndSize(inputSoundfile);
    
    //Process the buffer in chunks called buffer size at a time
    framesread = psf_sndReadFloatFrames(inputSoundfile, frame,bufferSize);

    //printf("\nThe total amount of frames is -> %d\n", framesread);
    
    //printf("\nThe total amount of frames is -> %d\n", psf_sndSize(inputSoundfile));

    //Counter to know how many blocks of frames were processed in total
    totalread = 0;

    //Processing the frames with a buffer size only when the buffer size's reminder is 0
    while (framesread == bufferSize)
    {
        totalread += bufferSize;
        if (psf_sndWriteFloatFrames(outputSoundfile,frame,bufferSize) != bufferSize)
        {
            printf("Error wiritng to outfile\n");
            error++;
            break;
        }

        framesread = psf_sndReadFloatFrames(inputSoundfile, frame, bufferSize);
        printf("\n\tTotal read of chunk is: %d\n", totalread);
    }

    //Processing the remaining frames when the frames read is less than the buffer size
    //The total read and the frames in the input soundfile are not the same.
    if (totalread != totalFramesInputFile)
    {
        //framesread stores the number of remaining frames
        int remainingFrames = framesread;
        
        //Updates de total read variable so that it can be printed later
        totalread += remainingFrames;

        if (psf_sndWriteFloatFrames(outputSoundfile, frame, remainingFrames) != remainingFrames)
        {
            printf("Error writing to outfile\n");
            error++;
        }

        //Process the remaining frames
        framesread = psf_sndReadFloatFrames(inputSoundfile, frame, remainingFrames);

        if (psf_sndReadPeaks(outputSoundfile, peaks, NULL) > 0)
        {
            long i;
            double peaktime;
            for (i = 0; i < props.chans; i++)
            {
                peaktime = (double)peaks[i].pos / props.srate;
                printf("CH %d:\t%.4f at %.4f secs\n", i + 1, peaks[i].val, peaktime);
            }
        }
    }
    if (framesread < 0)
    {
        printf("Error reading infile. Outfile is incomplete \n");
        error++;
    }
    else 
    {
        printf("\nDone %d sample frames copied to %s\n", totalread, outputFilePath);
    }
    

    printf("\nThe total frames read was: %d\n", totalread);
    printf("Properties of the output soundfile: \n");
    printf("\t-Sample Type: %s \n", ConvertToSampleType(props.samptype));
    printf("\t-Channels: %d \n", props.chans);
    printf("\t-Sample Rate: %d \n", props.srate);
    printf("\t-Format: %s \n", ConvertToFormat(props.format));
    printf("\t-Channel Format: %s \n", ConvertToChannelFormat(props.chformat));
    printf("\t-Total Frames: %d \n", psf_sndSize(outputSoundfile));

    //Closes the soundfile
    psf_sndClose(inputSoundfile);

    exit:
    if (inputSoundfile >= 0)
        psf_sndClose(inputSoundfile);
    if (outputSoundfile >= 0)
         psf_sndClose(outputSoundfile);
    if (frame)
        free(frame);
    if (peaks)
         free(peaks);
     psf_finish();
     return error;
}
