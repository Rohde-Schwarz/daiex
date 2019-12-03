// <copyright file="InterfaceTest.cs" company="Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich">
//   Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich. All rights reserved.
// </copyright>
//
//
// <summary>
//   Tests basic IDataImportExport interface methods of daiex wrapper.
// </summary>

namespace RohdeSchwarz.Mosaik.DataImportExportWrapperTest
{
  using System.Collections.Generic;
  using System.IO;
  using NUnit.Framework;
  using RohdeSchwarz.Mosaik.DataImportExport;

  [TestFixture]
  public class InterfaceTest
  {
    [Test]
    public void TestGetFileFormats()
    {
      Assert.AreEqual(FileTypeService.PossibleFileFormats.Count, 8 );
      Assert.True(FileTypeService.PossibleFileFormats.Contains(FileType.Csv));
      Assert.True(FileTypeService.PossibleFileFormats.Contains(FileType.Iqtar));
      Assert.True(FileTypeService.PossibleFileFormats.Contains(FileType.IQW));
      Assert.True(FileTypeService.PossibleFileFormats.Contains(FileType.Matlab4));
      Assert.True(FileTypeService.PossibleFileFormats.Contains(FileType.Matlab73));
      //Assert.True(FileTypeService.PossibleFileFormats.Contains(FileType.IQX));
     }

        [Test]
    public void TestGetFileExtension()
    {
      Assert.AreEqual(FileTypeService.GetFileExtension(FileType.Csv), "csv");
      Assert.AreEqual(FileTypeService.GetFileExtension(FileType.Iqtar), "iq.tar");
      Assert.AreEqual(FileTypeService.GetFileExtension(FileType.IQW), "iqw");
      Assert.AreEqual(FileTypeService.GetFileExtension(FileType.Matlab4), "mat");
      Assert.AreEqual(FileTypeService.GetFileExtension(FileType.Matlab73), "mat");
    }

    [TestCase(FileType.Csv)]
    [TestCase(FileType.Iqtar)]
    [TestCase(FileType.IQW)]
    [TestCase(FileType.Matlab4)]
    [TestCase(FileType.Matlab73)]
    public void TestFactoryArrays(FileType fileType)
    {
      string filename = Path.GetTempPath() + "TestFactoryArrays." + FileTypeService.GetFileExtension(fileType);
      
      IDataImportExport writeFile = FileTypeService.Create(filename, fileType);
      Assert.NotNull(writeFile, "IDataImportExport obj not created.");

      if (fileType == FileType.Matlab4)
      {
        ((IqMatlab)writeFile).MatlabFileVersion = MatlabVersion.Mat4;
      }
      else if (fileType == FileType.Matlab73)
      {
        ((IqMatlab)writeFile).MatlabFileVersion = MatlabVersion.Mat73;
      }

      IList<ChannelInfo> channelInfosWrite = new List<ChannelInfo>();
      channelInfosWrite.Add(new ChannelInfo("Channel1", 12.2, 13.3));

      IDictionary<string, string> metadataWrite = new Dictionary<string, string>();
      metadataWrite.Add(new KeyValuePair<string, string>("Key1", "value 1"));
      metadataWrite.Add(new KeyValuePair<string, string>("Key2", "value 2"));

      IList<IList<double>> data = Common.InitDoubleVector(2, 5);
      
      try
      {
        writeFile.WriteOpen(IqDataFormat.Complex, 2, "application name", "my comment", channelInfosWrite, metadataWrite);
        writeFile.AppendArrays(data);
        writeFile.AppendArrays(data);
        writeFile.Close();
      }
      catch (DaiException e)
      {
        Assert.Fail("Expected no exception, but got: " + e.Message);
      }

      IDataImportExport readFile = FileTypeService.Create(filename, fileType);
      Assert.NotNull(readFile, "IDataImportExport obj not created.");

      try
      {
        IList<string> arrayNames = readFile.ReadOpen();
        Assert.AreEqual(arrayNames.Count, 2, "invalid array count");
        Assert.AreEqual("Channel1_I", arrayNames[0], "invalid array name");
        Assert.AreEqual("Channel1_Q", arrayNames[1], "invalid array name");

        Assert.AreEqual(readFile.ChannelInformation.Count, channelInfosWrite.Count, "number of found channels incorrect");
        Assert.AreEqual(readFile.ChannelInformation[0].ChannelName, channelInfosWrite[0].ChannelName);

        if (fileType != FileType.IQW)
        {
          Assert.AreEqual(readFile.ChannelInformation[0].ClockRate, channelInfosWrite[0].ClockRate);
          Assert.AreEqual(readFile.ChannelInformation[0].Frequency, channelInfosWrite[0].Frequency);
          
          IDictionary<string, string> readMetadata = readFile.Metadata;
          foreach (var kvp in metadataWrite)
          {
            bool containsKey = readMetadata.ContainsKey(kvp.Key);
            Assert.True(containsKey, "metadata key not found");

            string value = readMetadata[kvp.Key];
            Assert.AreEqual(value, kvp.Value, "metadata value mismatch");
          }
        }
        
        int arrayLen = (int)readFile.GetArraySize(arrayNames[0]);

        IList<double> ivalues = new List<double>(arrayLen);
        IList<double> qvalues = new List<double>(arrayLen);
        readFile.ReadArray(arrayNames[0], ref ivalues, arrayLen);
        readFile.ReadArray(arrayNames[1], ref qvalues, arrayLen);
        
        for (int i = 0; i < arrayLen / 2; ++i)
        {
          Assert.AreEqual(data[0][i], ivalues[i]);
          Assert.AreEqual(data[1][i], qvalues[i]);
        }

        for (int i = arrayLen / 2; i < arrayLen; ++i)
        {
          Assert.AreEqual(data[0][i - arrayLen/2], ivalues[i]);
          Assert.AreEqual(data[1][i - arrayLen/2], qvalues[i]);
        }

        readFile.Close();
      }
      catch (DaiException e)
      {
        Assert.Fail("Expected no exception, but got: " + e.Message);
      }

      File.Delete(filename);
    }

    [TestCase(FileType.Csv)]
    [TestCase(FileType.Iqtar)]
    [TestCase(FileType.IQW)]
    [TestCase(FileType.Matlab4)]
    [TestCase(FileType.Matlab73)]
    public void TestFactoryChannels(FileType fileType)
    {
      string filename = Path.GetTempPath() + "TestFactoryArrays." + FileTypeService.GetFileExtension(fileType);

      IDataImportExport writeFile = FileTypeService.Create(filename, fileType);
      Assert.NotNull(writeFile, "IDataImportExport obj not created.");

      if (fileType == FileType.Matlab4)
      {
        ((IqMatlab)writeFile).MatlabFileVersion = MatlabVersion.Mat4;
      }
      else if (fileType == FileType.Matlab73)
      {
        ((IqMatlab)writeFile).MatlabFileVersion = MatlabVersion.Mat73;
      }

      IList<ChannelInfo> channelInfosWrite = new List<ChannelInfo>();
      channelInfosWrite.Add(new ChannelInfo("Channel1", 12.2, 13.3));

      IDictionary<string, string> metadataWrite = new Dictionary<string, string>();
      metadataWrite.Add(new KeyValuePair<string, string>("Key1", "value 1"));
      metadataWrite.Add(new KeyValuePair<string, string>("Key2", "value 2"));

      IList<IList<double>> data = Common.InitDoubleVector(1, 6);

      try
      {
        writeFile.WriteOpen(IqDataFormat.Complex, 1, "application name", "my comment", channelInfosWrite, metadataWrite);
        writeFile.AppendChannels(data);
        writeFile.AppendChannels(data);
        writeFile.Close();
      }
      catch (DaiException e)
      {
        Assert.Fail("Expected no exception, but got: " + e.Message);
      }

      IDataImportExport readFile = FileTypeService.Create(filename, fileType);
      Assert.NotNull(readFile, "IDataImportExport obj not created.");

      try
      {
        IList<string> arrayNames = readFile.ReadOpen();
        Assert.AreEqual(arrayNames.Count, 2, "invalid array count");
        Assert.AreEqual("Channel1_I", arrayNames[0], "invalid array name");
        Assert.AreEqual("Channel1_Q", arrayNames[1], "invalid array name");

        Assert.AreEqual(readFile.ChannelInformation.Count, channelInfosWrite.Count, "number of found channels incorrect");
        Assert.AreEqual(readFile.ChannelInformation[0].ChannelName, channelInfosWrite[0].ChannelName);

        if (fileType != FileType.IQW)
        {
          Assert.AreEqual(readFile.ChannelInformation[0].ClockRate, channelInfosWrite[0].ClockRate);
          Assert.AreEqual(readFile.ChannelInformation[0].Frequency, channelInfosWrite[0].Frequency);

          IDictionary<string, string> readMetadata = readFile.Metadata;
          foreach (var kvp in metadataWrite)
          {
            bool containsKey = readMetadata.ContainsKey(kvp.Key);
            Assert.True(containsKey, "metadata key not found");

            string value = readMetadata[kvp.Key];
            Assert.AreEqual(value, kvp.Value, "metadata value mismatch");
          }
        }

        int arrayLen = (int)readFile.GetArraySize(arrayNames[0]) * 2;

        IList<double> values = new List<double>(arrayLen);
        readFile.ReadChannel("Channel1", ref values, arrayLen);

        for (int i = 0; i < arrayLen / 2; ++i)
        {
          Assert.AreEqual(data[0][i], values[i]);
        }

        for (int i = arrayLen / 2; i < arrayLen; ++i)
        {
          Assert.AreEqual(data[0][i - arrayLen / 2], values[i]);
        }

        readFile.Close();
      }
      catch (DaiException e)
      {
        Assert.Fail("Expected no exception, but got: " + e.Message);
      }

      File.Delete(filename);
    }

    [TestCase(FileType.Csv)]
    [TestCase(FileType.Iqtar)]
    [TestCase(FileType.IQW)]
    [TestCase(FileType.Matlab4)]
    [TestCase(FileType.Matlab73)]
    public void TestUTF8Filenames(FileType fileType)
    {
      string filename = Path.GetTempPath() + "записано." + FileTypeService.GetFileExtension(fileType);

      IDataImportExport writeFile = FileTypeService.Create(filename, fileType);
      Assert.NotNull(writeFile, "IDataImportExport obj not created.");

      if (fileType == FileType.Matlab4)
      {
        ((IqMatlab)writeFile).MatlabFileVersion = MatlabVersion.Mat4;
      }
      else if (fileType == FileType.Matlab73)
      {
        ((IqMatlab)writeFile).MatlabFileVersion = MatlabVersion.Mat73;
      }

      IList<ChannelInfo> channelInfosWrite = new List<ChannelInfo>();
      channelInfosWrite.Add(new ChannelInfo("Channel1", 12.2, 13.3));

      IDictionary<string, string> metadataWrite = new Dictionary<string, string>();
      metadataWrite.Add(new KeyValuePair<string, string>("Key1", "value 1"));
      metadataWrite.Add(new KeyValuePair<string, string>("Key2", "value 2"));

      IList<IList<double>> data = Common.InitDoubleVector(1, 6);

      try
      {
        writeFile.WriteOpen(IqDataFormat.Complex, 1, "application name", "my comment", channelInfosWrite, metadataWrite);
        writeFile.AppendChannels(data);
        writeFile.AppendChannels(data);
        writeFile.Close();
      }
      catch (DaiException e)
      {
        Assert.Fail("Expected no exception, but got: " + e.Message);
      }

      bool fileExists = File.Exists(filename);
      Assert.IsTrue(fileExists, "File was not created");

      File.Delete(filename);
    }
  }
}