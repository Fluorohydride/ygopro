--地縛大神官
function c67987302.initial_effect(c)
	--indes
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e1:SetTarget(aux.TargetBoolFunction(Card.IsSetCard,0x21))
	e1:SetValue(c67987302.efilter)
	c:RegisterEffect(e1)
end
function c67987302.efilter(e,re,rp,c)
	return re:GetHandler()==c
end
