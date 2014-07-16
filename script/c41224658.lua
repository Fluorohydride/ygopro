--野望のゴーファー
function c41224658.initial_effect(c)
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(41224658,0))
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetTarget(c41224658.destg)
	e1:SetOperation(c41224658.desop)
	c:RegisterEffect(e1)
end
function c41224658.destg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and chkc:IsDestructable() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsDestructable,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,Card.IsDestructable,tp,0,LOCATION_MZONE,1,2,nil)
	if Duel.GetFieldGroupCount(tp,0,LOCATION_HAND)==Duel.GetMatchingGroupCount(Card.IsPublic,tp,0,LOCATION_HAND,nil)
		and not Duel.IsExistingMatchingCard(Card.IsType,tp,0,LOCATION_HAND,1,nil,TYPE_MONSTER) then
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
	end
end
function c41224658.cfilter(c)
	return not c:IsPublic() and c:IsType(TYPE_MONSTER)
end
function c41224658.desop(e,tp,eg,ep,ev,re,r,rp)
	local dis=false
	if Duel.IsChainDisablable(0) then
		local cg=Duel.GetMatchingGroup(c41224658.cfilter,tp,0,LOCATION_HAND,nil)
		if cg:GetCount()>0 and Duel.SelectYesNo(1-tp,aux.Stringid(41224658,1)) then
			Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_CONFIRM)
			local sg=cg:Select(1-tp,1,1,nil)
			Duel.ConfirmCards(tp,sg)
			Duel.ShuffleHand(1-tp)
			return
		end
	end
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(Card.IsRelateToEffect,nil,e)
	Duel.Destroy(g,REASON_EFFECT)
end
