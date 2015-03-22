--ハンター・アウル
function c51962254.initial_effect(c)
	--cannot be attacked
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_CANNOT_BE_BATTLE_TARGET)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c51962254.atcon)
	e1:SetValue(aux.imval1)
	c:RegisterEffect(e1)
	--atkup
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetCode(EFFECT_UPDATE_ATTACK)
	e2:SetRange(LOCATION_MZONE)
	e2:SetValue(c51962254.upval)
	c:RegisterEffect(e2)
end
function c51962254.cfilter(c)
	return c:IsFaceup() and c:IsAttribute(ATTRIBUTE_WIND)
end
function c51962254.atcon(e)
	return Duel.IsExistingMatchingCard(c51962254.cfilter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,e:GetHandler())
end
function c51962254.upval(e,c)
	return Duel.GetMatchingGroupCount(c51962254.upfilter,c:GetControler(),LOCATION_MZONE,0,nil)*500
end
function c51962254.upfilter(c)
	return c:IsFaceup() and c:IsAttribute(ATTRIBUTE_WIND)
end
