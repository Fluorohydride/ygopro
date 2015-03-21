--ホルスのしもべ
function c9264485.initial_effect(c)
	--cannot be target
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_BE_EFFECT_TARGET)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e1:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
	e1:SetTarget(c9264485.etarget)
	e1:SetValue(aux.tgoval)
	c:RegisterEffect(e1)
end
function c9264485.etarget(e,c)
	local code=c:GetCode()
	return code==75830094 or code==11224103 or code==48229808
end
