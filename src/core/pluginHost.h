/* -----------------------------------------------------------------------------
 *
 * Giada - Your Hardcore Loopmachine
 *
 * pluginHost
 *
 * -----------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2017 Giovanni A. Zuliani | Monocasual
 *
 * This file is part of Giada - Your Hardcore Loopmachine.
 *
 * Giada - Your Hardcore Loopmachine is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * Giada - Your Hardcore Loopmachine is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Giada - Your Hardcore Loopmachine. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * -------------------------------------------------------------------------- */

#ifdef WITH_VST

#ifndef G_PLUGIN_HOST_H
#define G_PLUGIN_HOST_H


#include <pthread.h>
#include "../deps/juce-config.h"


class Plugin;
class Channel;


class PluginHost
{
private:

  juce::MessageManager *messageManager;

  /* pluginFormat
   * Plugin format manager. */

  juce::VSTPluginFormat pluginFormat;

  /* knownPuginList
   * List of known (i.e. scanned) plugins. */

  juce::KnownPluginList knownPluginList;

  /* unknownPluginList
   * List of unrecognized plugins found in a patch. */

  std::vector<std::string> unknownPluginList;

  std::vector<Plugin*> masterOut;
  std::vector<Plugin*> masterIn;

  /* Audio|MidiBuffer
   * Dynamic buffers. */

  juce::AudioBuffer<float> audioBuffer;

  int samplerate;
  int buffersize;

  /* missingPlugins
   * If some plugins from any stack are missing. */

  bool missingPlugins;

public:

  enum stackType {
		MASTER_OUT,
		MASTER_IN,
		CHANNEL
	};

  enum sortMethod
  {
    NAME,
    CATEGORY,
    MANUFACTURER,
    FORMAT
  };

  struct PluginInfo
  {
    std::string uid;
    std::string name;
    std::string category;
    std::string manufacturerName;
    std::string format;
    bool isInstrument;
  };

  ~PluginHost();

  void init(int bufSize, int frequency);

  /* scanDir
   * Parse plugin directory and store list in knownPluginList. The callback is
   * called on each plugin found. Used to update the main window from the GUI
   * thread. */

  int scanDir(const std::string &path, void (*callback)(float progress, void *p)=nullptr,
      void *p=nullptr);

  /* (save|load)List
   * (Save|Load) knownPluginList (in|from) an XML file. */

  int saveList(const std::string &path);
  int loadList(const std::string &path);

  /* addPlugin
   * Add a new plugin to 'stackType' by unique id or by index in knownPluginList
   * std::vector. Requires:
   * fid - plugin unique file id (i.e. path to dynamic library)
   * stackType - which stack to add plugin to
   * mutex - Mixer.mutex_plugin
   * freq - current audio frequency
   * bufSize - buffer size
   * ch - if stackType == CHANNEL. */

  Plugin *addPlugin(const std::string &fid, int stackType, pthread_mutex_t *mutex,
    Channel *ch=nullptr);
  Plugin *addPlugin(int index, int stackType, pthread_mutex_t *mutex,
    Channel *ch=nullptr);

  /* countPlugins
   * Return size of 'stackType'. */

  unsigned countPlugins(int stackType, Channel *ch=nullptr);

  /* countAvailablePlugins
   * Return size of knownPluginList. */

  int countAvailablePlugins();

  /* countUnknownPlugins
   * Return size of unknownPluginList. */

  unsigned countUnknownPlugins();

  /* getAvailablePluginInfo
   * Return the available plugin information (name, type, ...) from
   * knownPluginList at index 'index'. */

  PluginInfo getAvailablePluginInfo(int index);

  std::string getUnknownPluginInfo(int index);

  /* freeStack
   * free plugin stack of type 'stackType'. */

  void freeStack(int stackType, pthread_mutex_t *mutex, Channel *ch=nullptr);

  /* processStack
   * apply the fx list to the buffer. */

  void processStack(float *buffer, int stackType, Channel *ch=nullptr);

  /* getStack
  * Return a std::vector <Plugin *> given the stackType. If stackType == CHANNEL
  * a pointer to Channel is also required. */

  std::vector <Plugin *> *getStack(int stackType, Channel *ch=nullptr);

  /* getPluginByIndex */

  Plugin *getPluginByIndex(int index, int stackType, Channel *ch=nullptr);

  /* getPluginIndex */

  int getPluginIndex(int id, int stackType, Channel *ch=nullptr);

  /* swapPlugin */

  void swapPlugin(unsigned indexA, unsigned indexB, int stackType,
    pthread_mutex_t *mutex, Channel *ch=nullptr);

  /* freePlugin.
  Returns the internal stack index of the deleted plugin. */

  int freePlugin(int id, int stackType, pthread_mutex_t *mutex,
    Channel *ch=nullptr);

  /* runDispatchLoop
   * Wakes up plugins' GUI manager for N milliseconds. */

  void runDispatchLoop();

  /* freeAllStacks
   * Frees everything. */

  void freeAllStacks(std::vector <Channel*> *channels, pthread_mutex_t *mutex);

  /* clonePlugin */

  int clonePlugin(Plugin *src, int stackType, pthread_mutex_t *mutex,
    Channel *ch);

  /* doesPluginExist */

  bool doesPluginExist(const std::string &fid);

  bool hasMissingPlugins() { return missingPlugins; };

  void sortPlugins(int sortMethod);

  pthread_mutex_t mutex_midi;
};
#endif

#endif // #ifdef WITH_VST
