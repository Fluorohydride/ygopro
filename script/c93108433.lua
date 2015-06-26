--モンスター回収
function c93108433.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TODECK+CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c93108433.target)
	e1:SetOperation(c93108433.activate)
	c:RegisterEffect(e1)
end
function c93108433.mfilter(c,tp)
	return c:IsAbleToDeck() and tp==c:GetOwner()
end
function c93108433.hfilter(c,tp)
	return tp~=c:GetOwner()
end
function c93108433.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c93108433.mfilter(chkc,tp) end
	if chk==0 then return Duel.IsPlayerCanDraw(tp) and Duel.GetFieldGroupCount(tp,LOCATION_HAND,0)>0
		and Duel.IsExistingTarget(c93108433.mfilter,tp,LOCATION_MZONE,0,1,e:GetHandler(),tp) 
		and not Duel.IsExistingMatchingCard(c93108433.hfilter,tp,LOCATION_HAND,0,1,nil,tp)
	end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local g1=Duel.SelectTarget(tp,c93108433.mfilter,tp,LOCATION_MZONE,0,1,1,nil,tp)
	local g2=Duel.GetFieldGroup(tp,LOCATION_HAND,0)
	g1:Merge(g2)
	Duel.SetOperationInfo(0,CATEGORY_TODECK,g1,g1:GetCount(),0,0)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,g2:GetCount())
end
function c93108433.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	local g=Duel.GetFieldGroup(tp,LOCATION_HAND,0)
	local ct=g:GetCount()
	if tc:IsRelateToEffect(e) and ct>0 then
		g:AddCard(tc)
		Duel.SendtoDeck(g,nil,2,REASON_EFFECT)
		Duel.ShuffleDeck(tp)
		if ct>0 then
			Duel.BreakEffect()
			Duel.Draw(tp,ct,REASON_EFFECT)
		end
	end
end
