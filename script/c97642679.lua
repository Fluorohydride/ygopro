--闇の支配者－ゾーク
function c97642679.initial_effect(c)
	c:EnableReviveLimit()
	--dice
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(97642679,0))
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_DICE)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetTarget(c97642679.target)
	e1:SetOperation(c97642679.operation)
	c:RegisterEffect(e1)
end
function c97642679.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DICE,nil,0,tp,1)
	local g1=Duel.GetMatchingGroup(Card.IsDestructable,tp,0,LOCATION_MZONE,nil)
	local g2=Duel.GetMatchingGroup(Card.IsDestructable,tp,LOCATION_MZONE,0,nil)
	if g1:GetCount()~=0 and g2:GetCount()~=0 then
		g1:Merge(g2)
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,g1,1,0,0)
	end
end
function c97642679.operation(e,tp,eg,ep,ev,re,r,rp)
	local d=Duel.TossDice(tp,1)
	if d==1 or d==2 then
		local g=Duel.GetMatchingGroup(Card.IsDestructable,tp,0,LOCATION_MZONE,nil)
		Duel.Destroy(g,REASON_EFFECT)
	elseif d==6 then
		local g=Duel.GetMatchingGroup(Card.IsDestructable,tp,LOCATION_MZONE,0,nil)
		Duel.Destroy(g,REASON_EFFECT)
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
		local g=Duel.SelectMatchingCard(tp,Card.IsDestructable,tp,0,LOCATION_MZONE,1,1,nil)
		Duel.Destroy(g,REASON_EFFECT)
	end
end
