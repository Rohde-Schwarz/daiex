// <copyright file="Common.cs" company="Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich">
//   Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich. All rights reserved.
// </copyright>
//
//
// <summary>
//   Helper methods.
// </summary>

namespace RohdeSchwarz.Mosaik.DataImportExportWrapperTest
{
  using System.Collections.Generic;
  using System.IO;
  using NUnit.Framework;
  using RohdeSchwarz.Mosaik.DataImportExport;

  public class Common
  {
    static public IList<IList<double>> InitDoubleVector(int nofArrays, int nofValues)
    {
      IList<IList<double>> data = new List<IList<double>>(nofArrays);
      for (int a = 0; a < nofArrays; ++a)
      {
        IList<double> values = new List<double>(nofValues);
        for (int v = 0; v < nofValues; ++v)
        {
          values.Add(a + v);
        }

        data.Add(values);
      }

      return data;
    }
  }

}