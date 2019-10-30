#!/usr/bin/env python
# encoding: utf-8

from load import ROOT as R
from pprint import pprint
import os
from os import path
import ordered_yaml
from stdvector import stdvector

from dagflow.node_deco import NodeInstanceMember, NodeInstanceStaticMember
from dagflow.membernode import MemberNodesHolder
from dagflow.graphviz import savegraph
from dagflow.wrappers import before_printer

try:
    OSError
except NameError:
    from exceptions import OSError

def touch_or_remove(fname, suffix, rm=False):
    assert suffix, 'Suffix should be not empty'
    filename = '.'.join((fname, suffix))

    if rm:
        try:
            os.remove(filename)
        except:
            pass
    else:
        fhandle = open(filename, 'a')
        try:
            os.utime(fname, None)
        finally:
            fhandle.close()

class analysisManager(MemberNodesHolder):
    """A main class processing dyb trees"""
    files = None # A sorted list of unique filenames
    files_root = ''
    file_iterator = None
    def __init__(self, opts):
        self.opts = opts

        MemberNodesHolder.__init__(self, graph='P19A IBD selection configuration')
        self.checkOpts()
        self.init()

    def checkOpts(self):
        from os.path import isdir, exists
        from os import makedirs
        try:
            if not exists(self.opts.output[0]):
                makedirs(self.opts.output[0])
        except:
            pass
        else:
            if isdir(self.opts.output[0]):
                return

        raise Exception('Unable to read or create directory: '+self.opts.output[0])

    def init(self):
        """Initialize configuration chain"""
        print('Initialize analysis chain')

        self.FileList >> self.InitAnalyzers >> self.SetupCuts   >> self.AnalyzeStage1 >> self.AnalyzeSingles
        self.FileList              >> self.CurrentFileIteration >> self.ParseFilename      >> self.AnalyzeStage1
        self.CurrentFileIteration  >> self.ReadInputFile        >> self.AnalyzeStage1
        self.FileList              >> self.FindCommonRoot       >> self.CreateOutputFile   >> self.AnalyzeStage1
        (self.CurrentFileIteration, self.ParseFilename)         >> self.CreateOutputFile


        # self.graph._wrap_fcns(before_printer)
        # self.AnalyzeStage1._unwrap_fcn()
        self.FileList.taint(True)

        if self.opts.graph:
            savegraph(self.graph, self.opts.graph[0], rankdir='TB')

    def print(self, level, *args, **kwargs):
        fcn = kwargs.pop('fcn', print)

        end=None
        if fcn is pprint:
            end = kwargs.pop('end', None)

        if self.opts.verbose>=level:
            fcn(*args, **kwargs)

            if end:
                print(end)

    @NodeInstanceStaticMember(label='Setup cuts', immediate=True, auto_freeze=True)
    def SetupCuts(self):
        # TODO

        # self.cuts = cuts.load_cutset(self.opts.cuts, self.opts.verbose)

        # for analysis in self.analysis_managers:
            # for cut, value in self.cuts.items():
                # if cut in ('Description',):
                    # continue

                # try:
                    # attr = getattr(analysis, cut)
                # except:
                    # raise Exception('Unable to set cut value {}={!s}'.format(cut, value))
                # attr(value)
        pass

    @NodeInstanceStaticMember(label='Init file list', immediate=True, auto_freeze=True)
    def FileList(self):
        files = self.opts.files
        if not files:
            f = open(self.opts.filelist, 'r')
            assert f, 'Can not open file '+self.filelist
            files = f.read().splitlines()

        self.files = list(sorted(set(files)))

        self.print(1, 'Input:', len(files), 'files (sorted by name)')
        self.print(2, self.files, fcn=pprint)

    @NodeInstanceStaticMember(label='Find common root', immediate=True, auto_freeze=True)
    def FindCommonRoot(self):
        it = iter(self.files)
        root = next(it).split('/')[:-1]
        for path in it:
            path = path.split('/')[:-1]
            if len(path)<len(root):
                root = root[:len(path)]

            if len(root)<len(path):
                path = path[:len(root)]

            newroot = root
            for i, (a, b) in enumerate(zip(root, path)):
                if a!=b:
                    newroot = newroot[:i]
                    break
            root = newroot
        root = '/'.join(root) or ''

        self.print(1, 'Common file root:', root)
        self.files_root = root

    @NodeInstanceStaticMember(label='Init analyzers', immediate=True, auto_freeze=True)
    def InitAnalyzers(self):
        p = self.pipeline = R.Pipeline()

        algs_pre   = ( 'EventReader', 'TrigTypeCut', 'MuonSaver', 'FlasherCut' )
        algs_ad    = ( 'ClusterSaver', )
        algs_post  = ( 'LivetimeSaver', )

        for alg in algs_pre:
            self.print(1, 'Add algorithm '+alg)
            p.makeAlg(alg)()

        # TODO
        # detectors = list(range(1, 5 if self.opts.site==3 else 2))
        detectors = list(range(1, 5))
        if algs_ad:
            for det in detectors:
                for alg in algs_ad:
                    self.print(1, 'Add AD algorithm {} ({})'.format(alg, det))
                    R.makeAlg(alg)(p, det)

        for alg in algs_post:
            self.print(1, 'Add algorithm {}'.format(alg))
            p.makeAlg(alg)()

    @NodeInstanceStaticMember(label='Next filename (loop)')
    def CurrentFileIteration(self):
        if not self.file_iterator:
            self.file_iterator = iter(self.files)

        self.current_filename = next(self.file_iterator)

    @NodeInstanceStaticMember(label='Analyze singles in a file')
    def AnalyzeStage1(self):
        """
        Load a single file, read data trees and set them to the TreeAnalyzer
        """
        nevents=-1
        v = stdvector((self.current_filename,))
        self.pipeline.process(v)

        status = 'success'
        events = 'all'
        stop = False
        # if nevents==-1:
            # status='fail'
            # events='truncated'
            # stop = True
        # elif self.opts.events>-1 and nevents>=self.opts.events:
            # events='truncated'
            # stop = True

        touch_or_remove(self.outputfilename, 'truncated', events!='truncated')
        self.saveInfo('status', dict(status=status, events=events), auto_title=True)
        # if self.opts.verbose>2:
            # self.outputfile.ls()
        # self.outputfile.Close()

        print("\033[1K    ...done! Read %i events"%(nevents), end='\r')
        self.print(3, "\033[1K    ...done! Read %i events"%(nevents), end='\n\n')
        if stop:
            raise StopIteration()

    @NodeInstanceStaticMember(label='Analyze singles', touch_inputs=False)
    def AnalyzeSingles(self):
        try:
            while True:
                self.AnalyzeStage1.touch()
                self.CurrentFileIteration.taint()
        except StopIteration:
            self.CurrentFileIteration.invalid = True

        print("\033[1K", end='\r')

        print('Done processing. Processed {} events from {} files ({} skipped)'.format(-1, -1, -1))
            # self.analyzer.GetNProcessedEvents(), self.analyzer.GetNProcessedFiles(), self.analyzer.GetNIgnoredEvents()))

    @NodeInstanceStaticMember(label='Parse filename')
    def ParseFilename(self):
        from os.path import basename
        data=dict(filename=self.current_filename)

        parts = basename(self.current_filename).split('.')
        try:
            source, tag, run, datatype, site, dataset, filenumber, _ = parts
        except ValueError:
            raise Exception('Unable to parse filename {} ({} parts)'.format(self.current_filename, len(parts)))

        data['source']     = source
        data['tag']        = tag
        data['run']        = run
        data['datatype']   = datatype
        data['site']       = site
        data['dataset']    = dataset
        data['filenumber'] = filenumber

        def exception(message):
            print('Filename:', self.current_filename)
            print('Data:', data)
            raise Exception(message)

        try:
            data['run'] = run = int(run)
        except ValueError:
            exception('Invalid run {}'.format(run))

        try:
            if not filenumber[0]=='_':
                raise ValueError
            data['filenumber'] = filenumber = int(filenumber[1:])
        except ValueError:
            exception('Invalid filenumber {}'.format(filenumber))

        if source!='recon':
            exception('Invalid source {}'.format(source))

        if tag!='Neutrino':
            exception('Invalid tag {}'.format(tag))

        if datatype!='Physics':
            exception('Invalid datatype {}'.format(datatype))

        if site not in ('EH{}-Merged'.format(i) for i in (1,2,3)):
            exception('Invalid site {}'.format(site))

        data['site'] = site = site[:3]

        self.fileinfo = data
        self.print(3, data, fcn=pprint)

    @NodeInstanceStaticMember(label='Read input file')
    def ReadInputFile(self):
        print('\033[1KLooping over singles tree', self.current_filename, end='\r')
        self.print(2, 'Read input file:', self.current_filename)

        # # TODO
        # self.inputfile = R.TFile(self.current_filename, "READ")
        # assert self.inputfile and not self.inputfile.IsZombie(), 'Can not open root file '+self.current_filename

    def saveInfo(self, name, info, title='', print_threshold=3, **kwargs):
        auto_title = kwargs.pop('auto_title', False)
        assert not kwargs, 'Unparsed arguments: '+str(kwargs)

        data = ordered_yaml.ordered_dump(info)

        if auto_title:
            title = str(data)
            if title[-1]=='\n':
                title = title[:-1]
            title = title.replace('\n', '; ')

        if title:
            title = title+' (yaml)'
        else:
            title = 'YAML data'

        odata = R.TObjString(data)
        self.outputfile.WriteTObject(odata, name, 'overwrite')
        key = self.outputfile.GetKey(name)
        key.SetTitle(title)

        self.print(print_threshold, 'Save info as {name}: {title}'.format(name=name, title=title))
        self.print(print_threshold, data, end='\n')

    @NodeInstanceStaticMember(label='Create output file')
    def CreateOutputFile(self):
        if not self.current_filename.startswith(self.files_root):
            raise Exception('Invalid filename or invalid file root')

        base = self.current_filename[len(self.files_root):]
        if base[0]=='/':
            base = base[1:]

        dirname, basename = path.split(base)
        basename = '_'.join((self.opts.output[1], basename))
        dirname = path.join(self.opts.output[0], dirname)
        try:
            os.makedirs(dirname)
        except OSError:
            pass
        else:
            self.print(3, 'Create output folder:', dirname)

        output = path.join(dirname, basename)
        self.print(2, 'Create output file:', output)

        self.outputfilename = output
        self.outputfile = self.pipeline.makeOutFile(self.outputfilename, "", True)
        assert not self.outputfile.IsZombie(), 'Can not create output file: '+output

        self.saveInfo('fileinfo', self.fileinfo, 'Input file info')
        # self.saveInfo('cuts',     self.cuts, 'Cuts summary', 4)

    def run(self):
        self.AnalyzeSingles.touch()

        if len(self.opts.graph)>1:
            savegraph(self.graph, self.opts.graph[1], rankdir='TB')

if __name__ == '__main__':
    from argparse import ArgumentParser
    # R.gROOT.SetBatch( True )
    parser = ArgumentParser()
    parser_files = parser.add_mutually_exclusive_group(required=True)
    parser_files.add_argument('-l', '--list', help='A file, containing a list of files to process')
    parser_files.add_argument('-f', '--files', nargs='+', default=[], help='List of files to process')
    parser.add_argument('-o', '--output', required=True, nargs=2, default=('', ''), help='Output folder and file prefix')

    parser.add_argument('-a', '--analyses', default=[], nargs='+', help='List of analysis managers' )
    parser.add_argument('-v', '--verbose', action='count', default=0, help='Be more verbose' )
    parser.add_argument('-g', '--graph', nargs='+', default=(), help='Save the graph' )

    # parser.add_argument( '-c', '--cuts', required=True, choices=cuts.available_cutsets(), help='Set of cuts to load')
    # parser.add_argument( '-n', '--events', type=int, default=-1, help='Max number of events to analyze')

    opts = parser.parse_args()
    ta = analysisManager(opts)
    ta.run()

