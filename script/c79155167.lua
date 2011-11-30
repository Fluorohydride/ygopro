--アーマーブラスト
function c79155167.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c79155167.target)
	e1:SetOperation(c79155167.activate)
	c:RegisterEffect(e1)
end
function c79155167.filter1(c)
	return c:IsFaceup() and c:IsSetCard(0x56) and c:IsDestructable()
end
function c79155167.filter2(c)
	return c:IsFaceup() and c:IsDestructable()
end
function c79155167.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then return Duel.IsExistingTarget(c79155167.filter1,tp,LOCATION_ONFIELD,0,1,nil)
		and Duel.IsExistingTarget(c79155167.filter2,tp,0,LOCATION_ONFIELD,2,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g1=Duel.SelectTarget(tp,c79155167.filter1,tp,LOCATION_ONFIELD,0,1,1,nil)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g2=Duel.SelectTarget(tp,c79155167.filter2,tp,0,LOCATION_ONFIELD,2,2,nil)
	g1:Merge(g2)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g1,g1:GetCount(),0,0)
end
function c79155167.tgfilter(c,e)
	return c:IsFaceup() and c:IsRelateToEffect(e)
end
function c79155167.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local tg=g:Filter(c79155167.tgfilter,nil,e)
	if tg:GetCount()>0 then
		Duel.Destroy(tg,REASON_EFFECT)
	end
end
