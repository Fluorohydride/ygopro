--Avalon
function c82410600.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TODECK+CATEGORY_DRAW)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c82410600.target)
	e1:SetOperation(c82410600.activate)
	c:RegisterEffect(e1)
end
function c82410600.filter(c,set)
	return c:IsSetCard(0x107a) and c:IsType(TYPE_MONSTER) and c:IsAbleToRemove() and (not set or c:IsSetCard(set))
end
function c82410600.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsDestructable,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,e:GetHandler())
		and Duel.IsExistingTarget(c82410600.filter,tp,LOCATION_GRAVE,0,5,nil) 
		and Duel.IsExistingTarget(c82410600.filter,tp,LOCATION_GRAVE,0,1,nil,0xa7) 
		and Duel.IsExistingTarget(c82410600.filter,tp,LOCATION_GRAVE,0,1,nil,0xa8) 
	end
	
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local g1=Duel.SelectTarget(tp,c82410600.filter,tp,LOCATION_GRAVE,0,1,1,nil,0xa7)
	local g2=Duel.SelectTarget(tp,c82410600.filter,tp,LOCATION_GRAVE,0,1,1,nil,0xa8)
	local g=Duel.GetMatchingGroup(c82410600.filter,tp,LOCATION_GRAVE,0,nil)
	g:Sub(g1)
	g:Sub(g2)
	local tg=g:Select(tp,3,3,nil)
	local tc=g:GetFirst()
	while tc do 
		Duel.SetTargetCard(tc)
		tc=g:GetNext()
	end
	
	tg:Merge(g1)
	tg:Merge	(g2)
	
	local dg=Duel.GetMatchingGroup(Card.IsDestructable,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,nil)
	
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,tg,tg:GetCount(),0,0)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,dg,dg:GetCount(),0,0)
end
function c82410600.activate(e,tp,eg,ep,ev,re,r,rp)
	local tg=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	if not tg or tg:FilterCount(Card.IsRelateToEffect,nil,e)==0 then return end
	if Duel.Remove(tg,POS_FACEUP,REASON_EFFECT) then
		local g=Duel.GetMatchingGroup(Card.IsDestructable,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,nil)
		Duel.Destroy(g,REASON_EFFECT)
	end
end