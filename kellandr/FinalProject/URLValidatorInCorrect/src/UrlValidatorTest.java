/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


import junit.framework.TestCase;





/**
 * Performs Validation Test for url validations.
 *
 * @version $Revision: 1128446 $ $Date: 2011-05-27 13:29:27 -0700 (Fri, 27 May 2011) $
 */
public class UrlValidatorTest extends TestCase {

   private boolean printStatus = false;
   private boolean printIndex = false;//print index that indicates current scheme,host,port,path, query test were using.

   public UrlValidatorTest(String testName) {
      super(testName);
   }

   private void manualTestHelper(String url, boolean expected) {
	   
	   UrlValidator urlVal = new UrlValidator(null, null, UrlValidator.ALLOW_ALL_SCHEMES);
	   
	   boolean result = urlVal.isValid(url);
	   
	   if ( result != expected) {
		   System.out.println( "Fault found. UrlValidator returned " + result + " for " + url + " expected " + expected );
	   }
   }
   
   
   public void testManualTest()
   {
	   UrlValidator urlVal = new UrlValidator(null, null, UrlValidator.ALLOW_ALL_SCHEMES);
	   
	   manualTestHelper("http://www.home.com", true);
	   
	   manualTestHelper("ftp://www.home.com", true);
	   manualTestHelper("mailto://www.home.com", true);
	   manualTestHelper("https://www.home.com", true);
	   
	   manualTestHelper("http://www.home.io", true);
	   manualTestHelper("http://www.home.org", true);
	   manualTestHelper("http://www.home.net", true);
	   manualTestHelper("http://www.home.edu", true);
	   manualTestHelper("http://www.home.int", true);
	   manualTestHelper("http://www.home.gov", true);
	   manualTestHelper("http://www.home.mil", true);
	   
	   manualTestHelper("http://www.home.com:999", true);
	   manualTestHelper("http://www.home.com:1000", true);
	   
	   manualTestHelper("http://www.home.com/?action=view", true);
	   manualTestHelper("http://www.home.com/?action=view&name1=2", true);
	   manualTestHelper("http://www.home.com/?action=view;name1=2", true);
	   
	   manualTestHelper("https://192.168.1.0", true);	   
	   
	   manualTestHelper("http://0.0.0.0", true);
	   manualTestHelper("http://0.0.0.0", true);
	   manualTestHelper("http://0.0.0.0:80/test1", true);
	   	   
	   manualTestHelper("http://www.home.dhw", false);
	   manualTestHelper("http://www.home.com:-1", false);
   }
   
   
   public void testYourFirstPartition()
   {
	   
   }
   
   public void testYourSecondPartition(){
	   
   }
   
   
   public void testIsValid()
   {
	   
	   ResultPair[] schemeTest={new ResultPair("http://",true),
			 	   new ResultPair("h5tp://",false),
			           new ResultPair("",true),
			   	   new ResultPair("http:/",false),
			   	   new ResultPair("http//",false)};
	   ResultPair[] hostTest={new ResultPair("256.256.256.256", false), 
			   	  new ResultPair("100.100.100.100",true),
			   	  new ResultPair("www.amazon.com",true),
			   	  new ResultPair("www.amazon.c0m",false),
			   	  new ResultPair("506.com",true),
			   	  new ResultPair("",false)};
	   ResultPair[] portTest={new ResultPair(":1a1", false),
			   	  new ResultPair(":1000",true),
			   	  new ResultPair(":-50",false),
			   	  new ResultPair(":65535",true),
			   	  new ResultPair(":70000",false)};
	   
	   		//path / filename
	   		//query
	   		//fragment identifier
	   
	   //function to loop through arrays and combine elements
	   		//concatenates each partition element and &&'s boolean
	   
	   
   }
   
   public void testAnyOtherUnitTest()
   {
	   
   }
   /**
    * Create set of tests by taking the testUrlXXX arrays and
    * running through all possible permutations of their combinations.
    *
    * @param testObjects Used to create a url.
    */
   

}
