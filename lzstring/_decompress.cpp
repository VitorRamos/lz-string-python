#ifndef __LIBMYPY_H__
#define __LIBMYPY_H__

#include <Python.h>

#include <iostream>
#include <map>
#include <vector>
#include <math.h>
#include <vector>
#include <bits/stdc++.h>

using namespace std;

PyObject* decompress64(PyObject *self, PyObject *args);

char decompressfunc_docs[] = "C decompress";

PyMethodDef decompress_funcs[] = {
	{
        "decompress64",
		(PyCFunction)decompress64,
		METH_O,
		decompressfunc_docs
    },
	{
        NULL
    }
};

char decompressmod_docs[] = "This is decompress module.";

PyModuleDef decompress_mod = {
	PyModuleDef_HEAD_INIT,
	"cdecompress",
	decompressmod_docs,
	-1,
	decompress_funcs,
	NULL,
	NULL,
	NULL,
	NULL
};

PyMODINIT_FUNC PyInit_cdecompress(void) {
	return PyModule_Create(&decompress_mod);
}

const string keyStrBase64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
const string keyStrUriSafe = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-$";

string _decompress64(string compressed)
{
    size_t length = compressed.size();
    int resetValue = 32;

    map<int, string> dictionary;
    int enlargeIn = 4;
    int dictSize = 4;
    int numBits = 3;
    string entry = "";
    string result;

    int data_val = keyStrBase64.find(compressed[0]);
    int data_pos = resetValue;
    int data_idx = 1;

    for(int i=0; i<3; i++)
        dictionary[i] = string(1,(char)i);

    int bits = 0;
    int maxpower = 4;
    int power = 1;
    int resb;
    
    while(power != maxpower){
        resb = data_val & data_pos;
        data_pos >>= 1;
        if (data_pos == 0){
            data_pos = resetValue;
            data_val = keyStrBase64.find(compressed[data_idx]);
            data_idx += 1;
        }
        bits |= power ? (resb > 0) : 0;
        power <<= 1;
    }

    int next = bits;
    int c;

    if (next == 0){
        bits = 0;
        maxpower = 256;
        power = 1;
        while (power != maxpower){
            resb = data_val & data_pos;
            data_pos >>= 1;
            if (data_pos == 0){
                data_pos = resetValue;
                data_val = keyStrBase64.find(compressed[data_idx]);
                data_idx += 1;
            }
            bits |= (resb > 0) ? power : 0;
            power <<= 1;
        }
        c = bits;
    }
    else if (next == 1){
        bits = 0;
        maxpower = 65536;
        power = 1;
        while (power != maxpower){
            resb = data_val & data_pos;
            data_pos >>= 1;
            if (data_pos == 0){
                data_pos = resetValue;
                data_val = keyStrBase64.find(compressed[data_idx]);
                data_idx += 1;
            }
            bits |= (resb > 0) ? power : 0;
            power <<= 1;
        }
        c = bits;
    }
    else if (next == 2)
        return "";
    
    dictionary[3] = string(1,c);
    string w(1, char(c));
    result += string(1, char(c));
    int counter = 0;

    while (1){
        counter += 1;
        if (data_idx > length)
            return "";
        bits = 0;
        maxpower = pow(2, numBits);
        power = 1;
        while (power != maxpower){
            resb = data_val & data_pos;
            data_pos >>= 1;
            if (data_pos == 0){
                data_pos = resetValue;
                data_val = keyStrBase64.find(compressed[data_idx]);
                data_idx += 1;
            }
            bits |= (resb > 0) ? power : 0;
            power <<= 1;
        }
        c = bits;
        if (c == 0){
            bits = 0;
            maxpower = 256;
            power = 1;
            while (power != maxpower){
                resb = data_val & data_pos;
                data_pos >>= 1;
                if (data_pos == 0){
                    data_pos = resetValue;
                    data_val = keyStrBase64.find(compressed[data_idx]);
                    data_idx += 1;
                }
                bits |= (resb > 0) ? power : 0;
                power <<= 1;
            }
            dictionary[dictSize] = string(1,(char)bits);
            dictSize += 1;
            c = dictSize - 1;
            enlargeIn -= 1;
        }
        else if (c == 1){
            bits = 0;
            maxpower = 65536;
            power = 1;
            while (power != maxpower){
                resb = data_val & data_pos;
                data_pos >>= 1;
                if (data_pos == 0){
                    data_pos = resetValue;
                    data_val = keyStrBase64.find(compressed[data_idx]);
                    data_idx += 1;
                }
                bits |= (resb > 0) ? power : 0;
                power <<= 1;
            }
            dictionary[dictSize] = string(1,(char)bits);
            dictSize += 1;
            c = dictSize - 1;
            enlargeIn -= 1;
        }
        else if (c == 2)
            return result;

        if (enlargeIn == 0){
            enlargeIn = pow(2, numBits);
            numBits += 1;
        }

        if (dictionary.find(c) != dictionary.end())
            entry = dictionary[c];
        else{
            if (c == dictSize)
                entry = w + w[0];
            else
                return "";
        }
        result += entry;

        // Add w+entry[0] to the dictionary.
        dictionary[dictSize] = w + entry[0];
        dictSize += 1;
        enlargeIn -= 1;

        w = entry;
        if (enlargeIn == 0){
            enlargeIn = pow(2, numBits);
            numBits += 1;
        }
    }
}

PyObject* decompress64(PyObject* self, PyObject *ptr) {
    
    PyObject* str = PyUnicode_AsEncodedString(ptr, "utf-8", "~E~");
    string c1 = PyBytes_AS_STRING(str);
    string c2 = _decompress64(c1);
    PyObject* res = PyUnicode_FromString(c2.c_str());
    return res;
}

#endif