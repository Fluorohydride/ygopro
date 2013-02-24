--半月竜ラディウス
function c6483224.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_HAND)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetCondition(c6483224.spcon)
	e1:SetOperation(c6483224.spop)
	c:RegisterEffect(e1)
end
function c6483224.filter(c)
	return c:IsFaceup() and c:IsType(TYPE_XYZ)
end
function c6483224.spcon(e,c)
	if c==nil then return true end
	return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c6483224.filter,c:GetControler(),0,LOCATION_MZONE,1,nil)
end
function c6483224.spop(e,tp,eg,ep,ev,re,r,rp,c)
	--change base attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_CHANGE_LEVEL)
	e1:SetValue(8)
	e1:SetReset(RESET_EVENT+0xff0000)
	c:RegisterEffect(e1)
end
