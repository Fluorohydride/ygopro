--ミミミック
function c45651298.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCountLimit(1,45651298)
	e1:SetCondition(c45651298.spcon)
	c:RegisterEffect(e1)
end
function c45651298.filter(c)
	return c:IsFaceup() and c:GetLevel()==3
end
function c45651298.spcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	return 	Duel.GetFieldGroupCount(tp,0,LOCATION_MZONE,nil)>0
		and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c45651298.filter,tp,LOCATION_MZONE,0,1,nil)
end
