// <copyright file="SettingsTest.cs" company="Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich">
//   Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich. All rights reserved.
// </copyright>
//
//
// <summary>
//   Tests Settings class of daiex wrapper.
// </summary>

namespace RohdeSchwarz.Mosaik.DataImportExportWrapperTest
{
  using System.Collections.Generic;
  using System.IO;
  using NUnit.Framework;
  using RohdeSchwarz.Mosaik.DataImportExport;

  [TestFixture]
  public class SettingsTest
  {
    [Test]
    public void TestBufferSize()
    {
      uint value = 6123;
      Assert.AreNotEqual(value, Settings.BufferSize);
      Settings.BufferSize = value;
      Assert.AreEqual(value, Settings.BufferSize);
    }
  }
}