"Hardcoded data, copied from the original P17B LBNL fitter inputs"


class Hardcoded:
    "Data for a given phase"

    def __init__(self, phase):
        self.phase = phase

    def fastnBkg(self):
        return [0.843, 0.843, 0.638, 0.638, 0.053, 0.053, 0.053, 0.053]

    def fastnBkgErr(self):
        return [0.083, 0.083, 0.062, 0.062, 0.009, 0.009, 0.009, 0.009]

    def amcBkg(self):
        if self.phase == 1:
            return [0.29, 0.27, 0.30, 0.000, 0.24, 0.23, 0.23, 0.000]
        elif self.phase == 2:
            return [0.15, 0.15, 0.12, 0.14, 0.04, 0.03, 0.03, 0.04]
        else:
            return [0., 0.11, 0.09, 0.08, 0.02, 0.02, 0.03, 0.02]

    def amcBkgErr(self):
        if self.phase == 1:
            return [0.13, 0.12, 0.14, 0.000, 0.11, 0.10, 0.10, 0.000]
        elif self.phase == 2:
            return [0.07, 0.07, 0.06, 0.06, 0.02, 0.01, 0.02, 0.02]
        else:
            return [0.07, 0.05, 0.04, 0.04, 0.01, 0.01, 0.01, 0.01]

    def alphanBkg(self):
        if self.phase == 1:
            return [0.09, 0.07, 0.05, 0.00, 0.05, 0.04, 0.04, 0.00]
        elif self.phase == 2:
            return [0.08, 0.06, 0.04, 0.06, 0.04, 0.04, 0.03, 0.04]
        else:
            return [0.0, 0.05, 0.03, 0.06, 0.03, 0.03, 0.03, 0.03]

    def alphanBkgErr(self):
        if self.phase == 1:
            return [0.04, 0.04, 0.02, 0.04, 0.02, 0.02, 0.02, 0.03]
        elif self.phase == 2:
            return [0.04, 0.03, 0.02, 0.03, 0.02, 0.02, 0.02, 0.02]
        else:
            return [0.04, 0.03, 0.02, 0.03, 0.02, 0.02, 0.01, 0.02]

    def targetMass(self):
        return [19941, 19966, 19891, 19945, 19913, 19991, 19892, 19931]

    # unused
    def timestamps(self):
        return [1324684800, 1386374400, '2011-12-24 00:00:00']

    # unused
    def delayedEff(self):
        return [0.88] * 8

    # unused
    def powerErr(self):
        return [0.8] * 8

    # unused
    def totEffErr(self):
        return [0.3] * 8
