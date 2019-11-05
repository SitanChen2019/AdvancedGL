#ifndef EXPORTER_H
#define EXPORTER_H

#ifdef _MSC_VER
#define EXPORT_SYMBOL  __declspec(dllexport)
#else
#define EXPORT_SYMBOL 
#endif


#define _worker 
#endif
