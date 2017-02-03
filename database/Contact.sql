Create Database GtkDemo
Go

Use GtkDemo
Go

CREATE TABLE [Contact] (
	[Contact_Id] [int] IDENTITY (1, 1) NOT NULL ,
	[FirstName] [varchar] (30) COLLATE SQL_Latin1_General_CP1_CI_AS NOT NULL CONSTRAINT [DF_Contact_FirstName] DEFAULT (''),
	[LastName] [varchar] (30) COLLATE SQL_Latin1_General_CP1_CI_AS NOT NULL CONSTRAINT [DF_Contact_LastName] DEFAULT (''),
	[PhoneNumber] [varchar] (30) COLLATE SQL_Latin1_General_CP1_CI_AS NOT NULL CONSTRAINT [DF_Contact_PhoneNumber] DEFAULT (''),
	[Email] [varchar] (30) COLLATE SQL_Latin1_General_CP1_CI_AS NOT NULL CONSTRAINT [DF_Contact_Email] DEFAULT (''),
	CONSTRAINT [PK_Contact] PRIMARY KEY  CLUSTERED
	(
		[Contact_Id]
	)  ON [PRIMARY]
) ON [PRIMARY]
GO


