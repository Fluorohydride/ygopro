--ディメンション・ポッド
function c73414375.initial_effect(c)
	--flip
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(73414375,0))
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_FLIP)
	e1:SetTarget(c73414375.target)
	e1:SetOperation(c73414375.operation)
	c:RegisterEffect(e1)
end
function c73414375.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then return true end
	local g=Group.CreateGroup()
	if Duel.IsExistingTarget(Card.IsAbleToRemove,tp,0,LOCATION_GRAVE,1,nil) and Duel.SelectYesNo(tp,aux.Stringid(73414375,1)) then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
		local rg=Duel.SelectTarget(tp,Card.IsAbleToRemove,tp,0,LOCATION_GRAVE,1,3,nil)
		g:Merge(rg)
	end
	if Duel.IsExistingTarget(Card.IsAbleToRemove,1-tp,0,LOCATION_GRAVE,1,nil) and Duel.SelectYesNo(1-tp,aux.Stringid(73414375,1)) then
		Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_REMOVE)
		local rg=Duel.SelectTarget(1-tp,Card.IsAbleToRemove,1-tp,0,LOCATION_GRAVE,1,3,nil)
		g:Merge(rg)
	end
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,g:GetCount(),0,0)
end
function c73414375.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	if not g then return end
	local rg=g:Filter(Card.IsRelateToEffect,nil,e)
	Duel.Remove(rg,POS_FACEUP,REASON_EFFECT)
end
