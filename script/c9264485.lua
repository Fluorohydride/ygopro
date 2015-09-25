--ホルスのしもべ
function c9264485.initial_effect(c)
	--cannot be target
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_SELECT_EFFECT_TARGET)
	e1:SetProperty(EFFECT_FLAG_SET_AVAILABLE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(0,0xff)
	e1:SetValue(c9264485.etarget)
	c:RegisterEffect(e1)
end
function c9264485.etarget(e,re,c)
	local code=c:GetCode()
	return c:IsLocation(LOCATION_MZONE) and c:IsFaceup() and (code==75830094 or code==11224103 or code==48229808)
end
