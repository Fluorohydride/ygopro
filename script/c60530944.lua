--異種闘争
function c60530944.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOGRAVE)
	e1:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1c0)
	e1:SetCondition(c60530944.condition)
	e1:SetOperation(c60530944.operation)
	c:RegisterEffect(e1)
end
function c60530944.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetFieldGroupCount(tp,LOCATION_MZONE,0)>0
		and not Duel.IsExistingMatchingCard(Card.IsFacedown,tp,LOCATION_MZONE,0,1,nil)
		and Duel.GetFieldGroupCount(tp,0,LOCATION_MZONE)>0
		and not Duel.IsExistingMatchingCard(Card.IsFacedown,tp,0,LOCATION_MZONE,1,nil)
end
function c60530944.getattr(g)
	local aat=0
	local tc=g:GetFirst()
	while tc do
		aat=bit.bor(aat,tc:GetAttribute())
		tc=g:GetNext()
	end
	return aat
end
function c60530944.rmfilter(c,at)
	return c:GetAttribute()==at
end
function c60530944.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetFieldGroupCount(tp,LOCATION_MZONE,0)==0
		or Duel.IsExistingMatchingCard(Card.IsFacedown,tp,LOCATION_MZONE,0,1,nil)
		or Duel.GetFieldGroupCount(tp,0,LOCATION_MZONE)==0
		or Duel.IsExistingMatchingCard(Card.IsFacedown,tp,0,LOCATION_MZONE,1,nil) then return end
	local g1=Duel.GetFieldGroup(tp,LOCATION_MZONE,0)
	local g2=Duel.GetFieldGroup(tp,0,LOCATION_MZONE)
	local c=e:GetHandler()
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(60530944,0))
	local r1=Duel.AnnounceAttribute(tp,1,c60530944.getattr(g1))
	g1:Remove(c60530944.rmfilter,nil,r1)
	Duel.Hint(HINT_SELECTMSG,1-tp,aux.Stringid(60530944,0))
	local r2=Duel.AnnounceAttribute(1-tp,1,c60530944.getattr(g2))
	g2:Remove(c60530944.rmfilter,nil,r2)
	g1:Merge(g2)
	Duel.SendtoGrave(g1,REASON_EFFECT)
end
