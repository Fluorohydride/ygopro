--フルハウス
function c45178472.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c45178472.target)
	e1:SetOperation(c45178472.activate)
	c:RegisterEffect(e1)
end
function c45178472.up(c)
	return c:IsFaceup() and c:IsDestructable() and c:IsType(TYPE_SPELL+TYPE_TRAP)
end
function c45178472.down(c)
	return c:IsFacedown() and c:IsDestructable() and c:IsType(TYPE_SPELL+TYPE_TRAP)
end
function c45178472.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then return Duel.IsExistingTarget(c45178472.up,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,2,e:GetHandler())
		and Duel.IsExistingTarget(c45178472.down,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,3,e:GetHandler()) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g1=Duel.SelectTarget(tp,c45178472.up,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,2,2,e:GetHandler())
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g2=Duel.SelectTarget(tp,c45178472.down,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,3,3,e:GetHandler())
	g1:Merge(g2)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g1,5,0,0)
end
function c45178472.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(Card.IsRelateToEffect,nil,e)
	Duel.Destroy(g,REASON_EFFECT)
end
