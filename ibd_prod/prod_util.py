import pandas as pd

DRL_PATH = '/global/homes/m/mkramer/projscratch/p17b/data_ana_lbnl/input/v3.merge/dbd_runlist.txt'

class DailyRunList:
    def __init__(self, path=DRL_PATH):
        self.df = pd.read_csv(path, sep='\t', header=None,
                              names=['day', 'site', 'runno', 'fileno'])

    def daily(self, *, site, day):
        sub_df = self.df.query(f'site == {site} and day == {day}')
        return sub_df[['runno', 'fileno']].values
