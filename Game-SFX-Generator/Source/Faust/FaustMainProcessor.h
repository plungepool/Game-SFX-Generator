/* ------------------------------------------------------------
name: "FaustSynth"
Code generated with Faust 2.23.4 (https://faust.grame.fr)
Compilation options: -lang cpp -scal -ftz 0
------------------------------------------------------------ */

#ifndef  __mydsp_H__
#define  __mydsp_H__

#include <cmath>
#include <cstring>

/************************** BEGIN MapUI.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef FAUST_MAPUI_H
#define FAUST_MAPUI_H

#include <vector>
#include <map>
#include <string>

/************************** BEGIN UI.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2020 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __UI_H__
#define __UI_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

/*******************************************************************************
 * UI : Faust DSP User Interface
 * User Interface as expected by the buildUserInterface() method of a DSP.
 * This abstract class contains only the method that the Faust compiler can
 * generate to describe a DSP user interface.
 ******************************************************************************/

struct Soundfile;

template <typename REAL>
struct UIReal
{
    UIReal() {}
    virtual ~UIReal() {}
    
    // -- widget's layouts
    
    virtual void openTabBox(const char* label) = 0;
    virtual void openHorizontalBox(const char* label) = 0;
    virtual void openVerticalBox(const char* label) = 0;
    virtual void closeBox() = 0;
    
    // -- active widgets
    
    virtual void addButton(const char* label, REAL* zone) = 0;
    virtual void addCheckButton(const char* label, REAL* zone) = 0;
    virtual void addVerticalSlider(const char* label, REAL* zone, REAL init, REAL min, REAL max, REAL step) = 0;
    virtual void addHorizontalSlider(const char* label, REAL* zone, REAL init, REAL min, REAL max, REAL step) = 0;
    virtual void addNumEntry(const char* label, REAL* zone, REAL init, REAL min, REAL max, REAL step) = 0;
    
    // -- passive widgets
    
    virtual void addHorizontalBargraph(const char* label, REAL* zone, REAL min, REAL max) = 0;
    virtual void addVerticalBargraph(const char* label, REAL* zone, REAL min, REAL max) = 0;
    
    // -- soundfiles
    
    virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) = 0;
    
    // -- metadata declarations
    
    virtual void declare(REAL* zone, const char* key, const char* val) {}
};

struct UI : public UIReal<FAUSTFLOAT>
{
    UI() {}
    virtual ~UI() {}
};

#endif
/**************************  END  UI.h **************************/
/************************** BEGIN PathBuilder.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef FAUST_PATHBUILDER_H
#define FAUST_PATHBUILDER_H

#include <vector>
#include <string>
#include <algorithm>

/*******************************************************************************
 * PathBuilder : Faust User Interface
 * Helper class to build complete hierarchical path for UI items.
 ******************************************************************************/

class PathBuilder
{

    protected:
    
        std::vector<std::string> fControlsLevel;
       
    public:
    
        PathBuilder() {}
        virtual ~PathBuilder() {}
    
        std::string buildPath(const std::string& label) 
        {
            std::string res = "/";
            for (size_t i = 0; i < fControlsLevel.size(); i++) {
                res += fControlsLevel[i];
                res += "/";
            }
            res += label;
            std::replace(res.begin(), res.end(), ' ', '_');
            return res;
        }
    
        void pushLabel(const std::string& label) { fControlsLevel.push_back(label); }
        void popLabel() { fControlsLevel.pop_back(); }
    
};

#endif  // FAUST_PATHBUILDER_H
/**************************  END  PathBuilder.h **************************/

/*******************************************************************************
 * MapUI : Faust User Interface
 * This class creates a map of complete hierarchical path and zones for each UI items.
 ******************************************************************************/

class MapUI : public UI, public PathBuilder
{
    
    protected:
    
        // Complete path map
        std::map<std::string, FAUSTFLOAT*> fPathZoneMap;
    
        // Label zone map
        std::map<std::string, FAUSTFLOAT*> fLabelZoneMap;
    
    public:
        
        MapUI() {}
        virtual ~MapUI() {}
        
        // -- widget's layouts
        void openTabBox(const char* label)
        {
            pushLabel(label);
        }
        void openHorizontalBox(const char* label)
        {
            pushLabel(label);
        }
        void openVerticalBox(const char* label)
        {
            pushLabel(label);
        }
        void closeBox()
        {
            popLabel();
        }
        
        // -- active widgets
        void addButton(const char* label, FAUSTFLOAT* zone)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addCheckButton(const char* label, FAUSTFLOAT* zone)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT fmin, FAUSTFLOAT fmax, FAUSTFLOAT step)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT fmin, FAUSTFLOAT fmax, FAUSTFLOAT step)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT fmin, FAUSTFLOAT fmax, FAUSTFLOAT step)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        
        // -- passive widgets
        void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT fmin, FAUSTFLOAT fmax)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT fmin, FAUSTFLOAT fmax)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
    
        // -- soundfiles
        virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) {}
        
        // -- metadata declarations
        void declare(FAUSTFLOAT* zone, const char* key, const char* val)
        {}
        
        // set/get
        void setParamValue(const std::string& path, FAUSTFLOAT value)
        {
            if (fPathZoneMap.find(path) != fPathZoneMap.end()) {
                *fPathZoneMap[path] = value;
            } else if (fLabelZoneMap.find(path) != fLabelZoneMap.end()) {
                *fLabelZoneMap[path] = value;
            }
        }
        
        FAUSTFLOAT getParamValue(const std::string& path)
        {
            if (fPathZoneMap.find(path) != fPathZoneMap.end()) {
                return *fPathZoneMap[path];
            } else if (fLabelZoneMap.find(path) != fLabelZoneMap.end()) {
                return *fLabelZoneMap[path];
            } else {
                return FAUSTFLOAT(0);
            }
        }
    
        // map access 
        std::map<std::string, FAUSTFLOAT*>& getMap() { return fPathZoneMap; }
        
        int getParamsCount() { return int(fPathZoneMap.size()); }
        
        std::string getParamAddress(int index)
        { 
            std::map<std::string, FAUSTFLOAT*>::iterator it = fPathZoneMap.begin();
            while (index-- > 0 && it++ != fPathZoneMap.end()) {}
            return (*it).first;
        }
    
        std::string getParamAddress(FAUSTFLOAT* zone)
        {
            std::map<std::string, FAUSTFLOAT*>::iterator it = fPathZoneMap.begin();
            do {
                if ((*it).second == zone) return (*it).first;
            }
            while (it++ != fPathZoneMap.end());
            return "";
        }
    
        static bool endsWith(std::string const& str, std::string const& end)
        {
            size_t l1 = str.length();
            size_t l2 = end.length();
            return (l1 >= l2) && (0 == str.compare(l1 - l2, l2, end));
        }
};


#endif // FAUST_MAPUI_H
/**************************  END  MapUI.h **************************/
/************************** BEGIN meta.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __meta__
#define __meta__

struct Meta
{
    virtual ~Meta() {};
    virtual void declare(const char* key, const char* value) = 0;
    
};

#endif
/**************************  END  meta.h **************************/
/************************** BEGIN dsp.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __dsp__
#define __dsp__

#include <string>
#include <vector>

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

struct UI;
struct Meta;

/**
 * DSP memory manager.
 */

struct dsp_memory_manager {
    
    virtual ~dsp_memory_manager() {}
    
    virtual void* allocate(size_t size) = 0;
    virtual void destroy(void* ptr) = 0;
    
};

/**
* Signal processor definition.
*/

class dsp {

    public:

        dsp() {}
        virtual ~dsp() {}

        /* Return instance number of audio inputs */
        virtual int getNumInputs() = 0;
    
        /* Return instance number of audio outputs */
        virtual int getNumOutputs() = 0;
    
        /**
         * Trigger the ui_interface parameter with instance specific calls
         * to 'addBtton', 'addVerticalSlider'... in order to build the UI.
         *
         * @param ui_interface - the user interface builder
         */
        virtual void buildUserInterface(UI* ui_interface) = 0;
    
        /* Returns the sample rate currently used by the instance */
        virtual int getSampleRate() = 0;
    
        /**
         * Global init, calls the following methods:
         * - static class 'classInit': static tables initialization
         * - 'instanceInit': constants and instance state initialization
         *
         * @param sample_rate - the sampling rate in Hertz
         */
        virtual void init(int sample_rate) = 0;

        /**
         * Init instance state
         *
         * @param sample_rate - the sampling rate in Hertz
         */
        virtual void instanceInit(int sample_rate) = 0;

        /**
         * Init instance constant state
         *
         * @param sample_rate - the sampling rate in Hertz
         */
        virtual void instanceConstants(int sample_rate) = 0;
    
        /* Init default control parameters values */
        virtual void instanceResetUserInterface() = 0;
    
        /* Init instance state (delay lines...) */
        virtual void instanceClear() = 0;
 
        /**
         * Return a clone of the instance.
         *
         * @return a copy of the instance on success, otherwise a null pointer.
         */
        virtual dsp* clone() = 0;
    
        /**
         * Trigger the Meta* parameter with instance specific calls to 'declare' (key, value) metadata.
         *
         * @param m - the Meta* meta user
         */
        virtual void metadata(Meta* m) = 0;
    
        /**
         * DSP instance computation, to be called with successive in/out audio buffers.
         *
         * @param count - the number of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         *
         */
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) = 0;
    
        /**
         * DSP instance computation: alternative method to be used by subclasses.
         *
         * @param date_usec - the timestamp in microsec given by audio driver.
         * @param count - the number of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (either float, double or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (either float, double or quad)
         *
         */
        virtual void compute(double /*date_usec*/, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { compute(count, inputs, outputs); }
       
};

/**
 * Generic DSP decorator.
 */

class decorator_dsp : public dsp {

    protected:

        dsp* fDSP;

    public:

        decorator_dsp(dsp* dsp = nullptr):fDSP(dsp) {}
        virtual ~decorator_dsp() { delete fDSP; }

        virtual int getNumInputs() { return fDSP->getNumInputs(); }
        virtual int getNumOutputs() { return fDSP->getNumOutputs(); }
        virtual void buildUserInterface(UI* ui_interface) { fDSP->buildUserInterface(ui_interface); }
        virtual int getSampleRate() { return fDSP->getSampleRate(); }
        virtual void init(int sample_rate) { fDSP->init(sample_rate); }
        virtual void instanceInit(int sample_rate) { fDSP->instanceInit(sample_rate); }
        virtual void instanceConstants(int sample_rate) { fDSP->instanceConstants(sample_rate); }
        virtual void instanceResetUserInterface() { fDSP->instanceResetUserInterface(); }
        virtual void instanceClear() { fDSP->instanceClear(); }
        virtual decorator_dsp* clone() { return new decorator_dsp(fDSP->clone()); }
        virtual void metadata(Meta* m) { fDSP->metadata(m); }
        // Beware: subclasses usually have to overload the two 'compute' methods
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { fDSP->compute(count, inputs, outputs); }
        virtual void compute(double date_usec, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { fDSP->compute(date_usec, count, inputs, outputs); }
    
};

/**
 * DSP factory class.
 */

class dsp_factory {
    
    protected:
    
        // So that to force sub-classes to use deleteDSPFactory(dsp_factory* factory);
        virtual ~dsp_factory() {}
    
    public:
    
        virtual std::string getName() = 0;
        virtual std::string getSHAKey() = 0;
        virtual std::string getDSPCode() = 0;
        virtual std::string getCompileOptions() = 0;
        virtual std::vector<std::string> getLibraryList() = 0;
        virtual std::vector<std::string> getIncludePathnames() = 0;
    
        virtual dsp* createDSPInstance() = 0;
    
        virtual void setMemoryManager(dsp_memory_manager* manager) = 0;
        virtual dsp_memory_manager* getMemoryManager() = 0;
    
};

/**
 * On Intel set FZ (Flush to Zero) and DAZ (Denormals Are Zero)
 * flags to avoid costly denormals.
 */

#ifdef __SSE__
    #include <xmmintrin.h>
    #ifdef __SSE2__
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8040)
    #else
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8000)
    #endif
#else
    #define AVOIDDENORMALS
#endif

#endif
/**************************  END  dsp.h **************************/

// BEGIN-FAUSTDSP

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <math.h>


#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

class mydsp : public dsp {
	
 private:
	
	int fSampleRate;
	float fConst0;
	float fConst1;
	FAUSTFLOAT fEntry0;
	float fConst2;
	float fRec0[2];
	FAUSTFLOAT fCheckbox0;
	float fVec0[2];
	int iVec1[2];
	int iRec2[2];
	FAUSTFLOAT fHslider0;
	float fRec3[2];
	FAUSTFLOAT fHslider1;
	float fRec4[2];
	float fConst3;
	float fRec1[2];
	int IOTA;
	float fVec2[131072];
	FAUSTFLOAT fHslider2;
	float fRec5[2];
	float fVec3[131072];
	FAUSTFLOAT fEntry1;
	FAUSTFLOAT fHslider3;
	float fRec9[2];
	float fRec8[2];
	FAUSTFLOAT fHslider4;
	float fRec10[2];
	float fRec7[2];
	FAUSTFLOAT fHslider5;
	float fRec11[2];
	FAUSTFLOAT fHslider6;
	float fRec13[2];
	float fRec12[2];
	float fRec6[2];
	float fVec4[131072];
	FAUSTFLOAT fHslider7;
	FAUSTFLOAT fHslider8;
	float fRec16[2];
	float fRec15[2];
	float fRec17[2];
	FAUSTFLOAT fHslider9;
	float fRec18[2];
	FAUSTFLOAT fHslider10;
	float fRec19[2];
	int iRec20[2];
	float fRec14[2];
	float fVec5[131072];
	float fVec6[131072];
	float fVec7[131072];
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.1");
		m->declare("compile_options", "-lang cpp -es 1 -single -ftz 0");
		m->declare("delays.lib/name", "Faust Delay Library");
		m->declare("delays.lib/version", "0.1");
		m->declare("envelopes.lib/adsr:author", "Yann Orlarey and Andrey Bundin");
		m->declare("envelopes.lib/author", "GRAME");
		m->declare("envelopes.lib/copyright", "GRAME");
		m->declare("envelopes.lib/license", "LGPL with exception");
		m->declare("envelopes.lib/name", "Faust Envelope Library");
		m->declare("envelopes.lib/version", "0.1");
		m->declare("filename", "GameSFX.dsp");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.3");
		m->declare("misceffects.lib/name", "Misc Effects Library");
		m->declare("misceffects.lib/version", "2.0");
		m->declare("name", "GameSFX");
		m->declare("oscillators.lib/name", "Faust Oscillator Library");
		m->declare("oscillators.lib/version", "0.1");
		m->declare("platform.lib/name", "Generic Platform Library");
		m->declare("platform.lib/version", "0.1");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
	}

	virtual int getNumInputs() {
		return 2;
	}
	virtual int getNumOutputs() {
		return 2;
	}
	
	static void classInit(int sample_rate) {
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		fConst0 = std::min<float>(192000.0f, std::max<float>(1.0f, float(fSampleRate)));
		fConst1 = (44.0999985f / fConst0);
		fConst2 = (1.0f - fConst1);
		fConst3 = (1.0f / fConst0);
	}
	
	virtual void instanceResetUserInterface() {
		fEntry0 = FAUSTFLOAT(1.0f);
		fCheckbox0 = FAUSTFLOAT(0.0f);
		fHslider0 = FAUSTFLOAT(0.29999999999999999f);
		fHslider1 = FAUSTFLOAT(7.0f);
		fHslider2 = FAUSTFLOAT(0.0f);
		fEntry1 = FAUSTFLOAT(0.0f);
		fHslider3 = FAUSTFLOAT(5.0f);
		fHslider4 = FAUSTFLOAT(0.5f);
		fHslider5 = FAUSTFLOAT(10.0f);
		fHslider6 = FAUSTFLOAT(60000.0f);
		fHslider7 = FAUSTFLOAT(10.0f);
		fHslider8 = FAUSTFLOAT(30000.0f);
		fHslider9 = FAUSTFLOAT(1.0f);
		fHslider10 = FAUSTFLOAT(1.0f);
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			fRec0[l0] = 0.0f;
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fVec0[l1] = 0.0f;
		}
		for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
			iVec1[l2] = 0;
		}
		for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			iRec2[l3] = 0;
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			fRec3[l4] = 0.0f;
		}
		for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			fRec4[l5] = 0.0f;
		}
		for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
			fRec1[l6] = 0.0f;
		}
		IOTA = 0;
		for (int l7 = 0; (l7 < 131072); l7 = (l7 + 1)) {
			fVec2[l7] = 0.0f;
		}
		for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
			fRec5[l8] = 0.0f;
		}
		for (int l9 = 0; (l9 < 131072); l9 = (l9 + 1)) {
			fVec3[l9] = 0.0f;
		}
		for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
			fRec9[l10] = 0.0f;
		}
		for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
			fRec8[l11] = 0.0f;
		}
		for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
			fRec10[l12] = 0.0f;
		}
		for (int l13 = 0; (l13 < 2); l13 = (l13 + 1)) {
			fRec7[l13] = 0.0f;
		}
		for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
			fRec11[l14] = 0.0f;
		}
		for (int l15 = 0; (l15 < 2); l15 = (l15 + 1)) {
			fRec13[l15] = 0.0f;
		}
		for (int l16 = 0; (l16 < 2); l16 = (l16 + 1)) {
			fRec12[l16] = 0.0f;
		}
		for (int l17 = 0; (l17 < 2); l17 = (l17 + 1)) {
			fRec6[l17] = 0.0f;
		}
		for (int l18 = 0; (l18 < 131072); l18 = (l18 + 1)) {
			fVec4[l18] = 0.0f;
		}
		for (int l19 = 0; (l19 < 2); l19 = (l19 + 1)) {
			fRec16[l19] = 0.0f;
		}
		for (int l20 = 0; (l20 < 2); l20 = (l20 + 1)) {
			fRec15[l20] = 0.0f;
		}
		for (int l21 = 0; (l21 < 2); l21 = (l21 + 1)) {
			fRec17[l21] = 0.0f;
		}
		for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
			fRec18[l22] = 0.0f;
		}
		for (int l23 = 0; (l23 < 2); l23 = (l23 + 1)) {
			fRec19[l23] = 0.0f;
		}
		for (int l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
			iRec20[l24] = 0;
		}
		for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
			fRec14[l25] = 0.0f;
		}
		for (int l26 = 0; (l26 < 131072); l26 = (l26 + 1)) {
			fVec5[l26] = 0.0f;
		}
		for (int l27 = 0; (l27 < 131072); l27 = (l27 + 1)) {
			fVec6[l27] = 0.0f;
		}
		for (int l28 = 0; (l28 < 131072); l28 = (l28 + 1)) {
			fVec7[l28] = 0.0f;
		}
	}
	
	virtual void init(int sample_rate) {
		classInit(sample_rate);
		instanceInit(sample_rate);
	}
	virtual void instanceInit(int sample_rate) {
		instanceConstants(sample_rate);
		instanceResetUserInterface();
		instanceClear();
	}
	
	virtual mydsp* clone() {
		return new mydsp();
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("GameSFX");
		ui_interface->openVerticalBox("00 GATE");
		ui_interface->addCheckButton("PLAY", &fCheckbox0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("01 Pitch");
		ui_interface->addHorizontalSlider("00 shift (semitones)", &fHslider2, 0.0f, -48.0f, 48.0f, 0.00999999978f);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("02 Vibrato");
		ui_interface->addHorizontalSlider("Delay", &fHslider6, 60000.0f, 0.0f, 384000.0f, 0.100000001f);
		ui_interface->addHorizontalSlider("Depth", &fHslider5, 10.0f, 0.0f, 20.0f, 0.100000001f);
		ui_interface->addHorizontalSlider("Duty", &fHslider4, 0.5f, 0.0f, 1.0f, 0.25f);
		ui_interface->addHorizontalSlider("Freq", &fHslider3, 5.0f, 0.0f, 25.0f, 0.100000001f);
		ui_interface->addNumEntry("LFO Shape", &fEntry1, 0.0f, 0.0f, 2.0f, 1.0f);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("03 Pitch Envelope");
		ui_interface->addHorizontalSlider("Delay", &fHslider8, 30000.0f, 0.0f, 384000.0f, 0.100000001f);
		ui_interface->addHorizontalSlider("Depth", &fHslider7, 10.0f, -20.0f, 20.0f, 0.100000001f);
		ui_interface->addHorizontalSlider("attack", &fHslider9, 1.0f, 0.0f, 10.0f, 0.00100000005f);
		ui_interface->addHorizontalSlider("decay", &fHslider10, 1.0f, 0.0f, 10.0f, 0.00100000005f);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("04 ADSR");
		ui_interface->addHorizontalSlider("attack", &fHslider0, 0.300000012f, 0.0f, 10.0f, 0.00100000005f);
		ui_interface->addHorizontalSlider("decay", &fHslider1, 7.0f, 0.0f, 10.0f, 0.00100000005f);
		ui_interface->closeBox();
		ui_interface->addNumEntry("gain", &fEntry0, 1.0f, 0.0f, 1.0f, 0.00100000005f);
		ui_interface->closeBox();
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* input1 = inputs[1];
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		float fSlow0 = (fConst1 * float(fEntry0));
		float fSlow1 = float(fCheckbox0);
		int iSlow2 = (fSlow1 > 0.0f);
		float fSlow3 = (fConst1 * float(fHslider0));
		float fSlow4 = (fConst1 * float(fHslider1));
		float fSlow5 = float(iSlow2);
		float fSlow6 = std::pow(2.0f, (0.0833333358f * float(fHslider2)));
		int iSlow7 = int(float(fEntry1));
		int iSlow8 = (iSlow7 == 0);
		int iSlow9 = (iSlow7 == 1);
		float fSlow10 = (fConst1 * float(fHslider3));
		float fSlow11 = (fConst1 * float(fHslider4));
		float fSlow12 = (fConst1 * float(fHslider5));
		int iSlow13 = (fSlow1 == 0.0f);
		int iSlow14 = (iSlow13 > 0);
		float fSlow15 = (fConst1 * float(fHslider6));
		float fSlow16 = (0.0833333358f * float(fHslider7));
		float fSlow17 = (fConst1 * float(fHslider8));
		float fSlow18 = (fConst1 * float(fHslider9));
		float fSlow19 = (fConst1 * float(fHslider10));
		for (int i0 = 0; (i0 < count); i0 = (i0 + 1)) {
			fRec0[0] = (fSlow0 + (fConst2 * fRec0[1]));
			fVec0[0] = fSlow1;
			iVec1[0] = iSlow2;
			iRec2[0] = (iSlow2 * (iRec2[1] + 1));
			fRec3[0] = (fSlow3 + (fConst2 * fRec3[1]));
			int iTemp0 = (iSlow2 - iVec1[1]);
			int iTemp1 = ((iRec2[0] < int((fConst0 * fRec3[0]))) | (iTemp0 * (iTemp0 > 0)));
			fRec4[0] = (fSlow4 + (fConst2 * fRec4[1]));
			float fTemp2 = (0.144717798f * (iSlow2 ? (iTemp1 ? fRec3[0] : fRec4[0]) : 0.0f));
			int iTemp3 = (std::fabs(fTemp2) < 1.1920929e-07f);
			float fTemp4 = (iTemp3 ? 0.0f : std::exp((0.0f - (fConst3 / (iTemp3 ? 1.0f : fTemp2)))));
			fRec1[0] = ((fRec1[1] * fTemp4) + ((iSlow2 ? (iTemp1 ? fSlow5 : 0.0f) : 0.0f) * (1.0f - fTemp4)));
			float fTemp5 = (fRec0[0] * fRec1[0]);
			float fTemp6 = float(input0[i0]);
			fVec2[(IOTA & 131071)] = fTemp6;
			fRec5[0] = std::fmod(((fRec5[1] + 1001.0f) - fSlow6), 1000.0f);
			int iTemp7 = int(fRec5[0]);
			int iTemp8 = std::min<int>(65537, std::max<int>(0, iTemp7));
			float fTemp9 = std::floor(fRec5[0]);
			float fTemp10 = (fTemp9 + (1.0f - fRec5[0]));
			float fTemp11 = (fRec5[0] - fTemp9);
			int iTemp12 = std::min<int>(65537, std::max<int>(0, (iTemp7 + 1)));
			float fTemp13 = std::min<float>((0.100000001f * fRec5[0]), 1.0f);
			float fTemp14 = (fRec5[0] + 1000.0f);
			int iTemp15 = int(fTemp14);
			int iTemp16 = std::min<int>(65537, std::max<int>(0, iTemp15));
			float fTemp17 = std::floor(fTemp14);
			float fTemp18 = (fTemp17 + (-999.0f - fRec5[0]));
			int iTemp19 = std::min<int>(65537, std::max<int>(0, (iTemp15 + 1)));
			float fTemp20 = (fRec5[0] + (1000.0f - fTemp17));
			float fTemp21 = (1.0f - fTemp13);
			float fTemp22 = ((((fVec2[((IOTA - iTemp8) & 131071)] * fTemp10) + (fTemp11 * fVec2[((IOTA - iTemp12) & 131071)])) * fTemp13) + (((fVec2[((IOTA - iTemp16) & 131071)] * fTemp18) + (fVec2[((IOTA - iTemp19) & 131071)] * fTemp20)) * fTemp21));
			fVec3[(IOTA & 131071)] = fTemp22;
			fRec9[0] = (fSlow10 + (fConst2 * fRec9[1]));
			float fTemp23 = (fRec8[1] + (fConst3 * fRec9[0]));
			fRec8[0] = (fTemp23 - std::floor(fTemp23));
			float fTemp24 = ((2.0f * fRec8[0]) + -1.0f);
			fRec10[0] = (fSlow11 + (fConst2 * fRec10[1]));
			fRec7[0] = ((fConst2 * fRec7[1]) + (fConst1 * (iSlow8 ? ((2.0f * float((fRec8[0] <= fRec10[0]))) + -1.0f) : (iSlow9 ? ((2.0f * (1.0f - std::fabs(fTemp24))) + -1.0f) : fTemp24))));
			fRec11[0] = (fSlow12 + (fConst2 * fRec11[1]));
			fRec13[0] = (fSlow15 + (fConst2 * fRec13[1]));
			fRec12[0] = (iSlow14 ? 0.0f : std::min<float>(fRec13[0], (fRec12[1] + 1.0f)));
			fRec6[0] = std::fmod((fRec6[1] + (1001.0f - std::pow(2.0f, (0.0833333358f * ((fRec7[0] * fRec11[0]) * float((((fRec12[0] < fRec13[0]) & (fRec12[0] != 0.0f)) ? 0 : 1))))))), 1000.0f);
			int iTemp25 = int(fRec6[0]);
			int iTemp26 = std::min<int>(65537, int(std::max<int>(0, int(iTemp25))));
			float fTemp27 = std::floor(fRec6[0]);
			float fTemp28 = (fTemp27 + (1.0f - fRec6[0]));
			float fTemp29 = (fRec6[0] - fTemp27);
			int iTemp30 = std::min<int>(65537, int(std::max<int>(0, int((iTemp25 + 1)))));
			float fTemp31 = std::min<float>((0.100000001f * fRec6[0]), 1.0f);
			float fTemp32 = (fRec6[0] + 1000.0f);
			int iTemp33 = int(fTemp32);
			int iTemp34 = std::min<int>(65537, int(std::max<int>(0, int(iTemp33))));
			float fTemp35 = std::floor(fTemp32);
			float fTemp36 = (fTemp35 + (-999.0f - fRec6[0]));
			float fTemp37 = (fRec6[0] + (1000.0f - fTemp35));
			int iTemp38 = std::min<int>(65537, int(std::max<int>(0, int((iTemp33 + 1)))));
			float fTemp39 = (1.0f - fTemp31);
			float fTemp40 = ((((fVec3[((IOTA - iTemp26) & 131071)] * fTemp28) + (fTemp29 * fVec3[((IOTA - iTemp30) & 131071)])) * fTemp31) + (((fVec3[((IOTA - iTemp34) & 131071)] * fTemp36) + (fTemp37 * fVec3[((IOTA - iTemp38) & 131071)])) * fTemp39));
			fVec4[(IOTA & 131071)] = fTemp40;
			fRec16[0] = (fSlow17 + (fConst2 * fRec16[1]));
			fRec15[0] = (iSlow14 ? 0.0f : std::min<float>(fRec16[0], (fRec15[1] + 1.0f)));
			fRec17[0] = (fSlow1 + (fRec17[1] * float((fVec0[1] >= fSlow1))));
			fRec18[0] = (fSlow18 + (fConst2 * fRec18[1]));
			float fTemp41 = std::max<float>(1.0f, (fConst0 * fRec18[0]));
			fRec19[0] = (fSlow19 + (fConst2 * fRec19[1]));
			iRec20[0] = (iSlow13 * (iRec20[1] + 1));
			fRec14[0] = std::fmod((fRec14[1] + (1001.0f - std::pow(2.0f, (fSlow16 * (float((((fRec15[0] < fRec16[0]) & (fRec15[0] != 0.0f)) ? 0 : 1)) * std::max<float>(0.0f, (std::min<float>((fRec17[0] / fTemp41), std::max<float>(((0.5f * ((fTemp41 - fRec17[0]) / std::max<float>(1.0f, (fConst0 * fRec19[0])))) + 1.0f), 0.5f)) * (1.0f - float(iRec20[0]))))))))), 1000.0f);
			int iTemp42 = int(fRec14[0]);
			int iTemp43 = std::min<int>(65537, int(std::max<int>(0, int(iTemp42))));
			float fTemp44 = std::floor(fRec14[0]);
			float fTemp45 = (fTemp44 + (1.0f - fRec14[0]));
			float fTemp46 = (fRec14[0] - fTemp44);
			int iTemp47 = std::min<int>(65537, int(std::max<int>(0, int((iTemp42 + 1)))));
			float fTemp48 = std::min<float>((0.100000001f * fRec14[0]), 1.0f);
			float fTemp49 = (fRec14[0] + 1000.0f);
			int iTemp50 = int(fTemp49);
			int iTemp51 = std::min<int>(65537, int(std::max<int>(0, int(iTemp50))));
			float fTemp52 = std::floor(fTemp49);
			float fTemp53 = (fTemp52 + (-999.0f - fRec14[0]));
			float fTemp54 = (fRec14[0] + (1000.0f - fTemp52));
			int iTemp55 = std::min<int>(65537, int(std::max<int>(0, int((iTemp50 + 1)))));
			float fTemp56 = (1.0f - fTemp48);
			output0[i0] = FAUSTFLOAT((fTemp5 * ((((fVec4[((IOTA - iTemp43) & 131071)] * fTemp45) + (fTemp46 * fVec4[((IOTA - iTemp47) & 131071)])) * fTemp48) + (((fVec4[((IOTA - iTemp51) & 131071)] * fTemp53) + (fTemp54 * fVec4[((IOTA - iTemp55) & 131071)])) * fTemp56))));
			float fTemp57 = float(input1[i0]);
			fVec5[(IOTA & 131071)] = fTemp57;
			float fTemp58 = ((fTemp13 * ((fVec5[((IOTA - iTemp8) & 131071)] * fTemp10) + (fTemp11 * fVec5[((IOTA - iTemp12) & 131071)]))) + (fTemp21 * ((fVec5[((IOTA - iTemp16) & 131071)] * fTemp18) + (fVec5[((IOTA - iTemp19) & 131071)] * fTemp20))));
			fVec6[(IOTA & 131071)] = fTemp58;
			float fTemp59 = ((fTemp31 * ((fTemp28 * fVec6[((IOTA - iTemp26) & 131071)]) + (fTemp29 * fVec6[((IOTA - iTemp30) & 131071)]))) + (fTemp39 * ((fTemp36 * fVec6[((IOTA - iTemp34) & 131071)]) + (fTemp37 * fVec6[((IOTA - iTemp38) & 131071)]))));
			fVec7[(IOTA & 131071)] = fTemp59;
			output1[i0] = FAUSTFLOAT((fTemp5 * ((fTemp48 * ((fTemp45 * fVec7[((IOTA - iTemp43) & 131071)]) + (fTemp46 * fVec7[((IOTA - iTemp47) & 131071)]))) + (fTemp56 * ((fTemp53 * fVec7[((IOTA - iTemp51) & 131071)]) + (fTemp54 * fVec7[((IOTA - iTemp55) & 131071)]))))));
			fRec0[1] = fRec0[0];
			fVec0[1] = fVec0[0];
			iVec1[1] = iVec1[0];
			iRec2[1] = iRec2[0];
			fRec3[1] = fRec3[0];
			fRec4[1] = fRec4[0];
			fRec1[1] = fRec1[0];
			IOTA = (IOTA + 1);
			fRec5[1] = fRec5[0];
			fRec9[1] = fRec9[0];
			fRec8[1] = fRec8[0];
			fRec10[1] = fRec10[0];
			fRec7[1] = fRec7[0];
			fRec11[1] = fRec11[0];
			fRec13[1] = fRec13[0];
			fRec12[1] = fRec12[0];
			fRec6[1] = fRec6[0];
			fRec16[1] = fRec16[0];
			fRec15[1] = fRec15[0];
			fRec17[1] = fRec17[0];
			fRec18[1] = fRec18[0];
			fRec19[1] = fRec19[0];
			iRec20[1] = iRec20[0];
			fRec14[1] = fRec14[0];
		}
	}

};

// END-FAUSTDSP

#endif
