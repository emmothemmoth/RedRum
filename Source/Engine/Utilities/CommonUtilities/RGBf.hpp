#pragma once
namespace CommonUtilities
{
	class RGBf
	{
	public:
		RGBf() = default;
		RGBf(const float& anR, const float& aG, const float& aB)
		{
			myR = anR;
			myG = aG;
			myB = aB;
		}
		~RGBf() = default;

		RGBf GetRGB() const
		{
			RGBf color(myR, myG, myB);
			return color;
		}
		float GetR() const
		{
			return myR;
		}
		float GetG() const
		{
			return myG;
		}
		float GetB() const
		{
			return myB;
		}
		RGBf SumColors(const RGBf& aColor, const RGBf& anotherColor)
		{
			RGBf colorOne = aColor.GetRGB();
			RGBf colorTwo = anotherColor.GetRGB();
			RGBf summedColor(0.0f, 0.0f, 0.0f);
			summedColor.myR = colorOne.myR + colorTwo.myR;
			summedColor.myG = colorOne.myG + colorTwo.myG;
			summedColor.myB = colorOne.myB + colorTwo.myB;
			return summedColor;
		}
		RGBf SubtractColors(const RGBf& aColor, const RGBf& anotherColor)
		{
			RGBf colorOne = aColor.GetRGB();
			RGBf colorTwo = anotherColor.GetRGB();
			RGBf subtractedColor(0.0f, 0.0f, 0.0f);
			subtractedColor.myR = colorOne.myR - colorTwo.myR;
			subtractedColor.myG = colorOne.myG - colorTwo.myG;
			subtractedColor.myB = colorOne.myB - colorTwo.myB;
			return subtractedColor;
		}

		void  operator=(const RGBf& aColor)
		{
			RGBf inputColor = aColor.GetRGB();
			myR = inputColor.myR;
			myG = inputColor.myG;
			myB = inputColor.myB;
		}
		RGBf operator+(const CommonUtilities::RGBf& aColor)
		{
			RGBf summedColor = summedColor.SumColors(*this, aColor);
			return summedColor;
		}
		RGBf operator-(const CommonUtilities::RGBf& aColor)
		{
			RGBf subtractedColor = subtractedColor.SubtractColors(*this, aColor);
			return subtractedColor;
		}
		RGBf operator+(const float anAmbientValue)
		{
			RGBf aNewColor(anAmbientValue, anAmbientValue, anAmbientValue);
			RGBf summedColor = summedColor.SumColors(*this, aNewColor);
			return summedColor;
		}

		float myR;
		float myG;
		float myB;
	private:
		
	};

}

CommonUtilities::RGBf operator+(const CommonUtilities::RGBf& aColor, CommonUtilities::RGBf& anotherColor)
{
	CommonUtilities::RGBf summedColor = summedColor.SumColors(aColor, anotherColor);
	return summedColor;
	
}
CommonUtilities::RGBf operator-(const CommonUtilities::RGBf& aColor, CommonUtilities::RGBf& anotherColor)
{
	CommonUtilities::RGBf subtractedColor = subtractedColor.SubtractColors(aColor, anotherColor);
	return subtractedColor;
}

