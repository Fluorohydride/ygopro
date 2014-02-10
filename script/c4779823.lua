--ライトロード・アーク ミカエル
function c4779823.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,nil,aux.NonTuner(Card.IsAttribute,ATTRIBUTE_LIGHT),1)
	c:EnableReviveLimit()
	--remove
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(4779823,0))
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCost(c4779823.rmcost)
	e1:SetTarget(c4779823.rmtg)
	e1:SetOperation(c4779823.rmop)
	c:RegisterEffect(e1)
	--todeck
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(4779823,1))
	e2:SetCategory(CATEGORY_TODECK+CATEGORY_RECOVER)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP)
	e2:SetCode(EVENT_DESTROYED)
	e2:SetTarget(c4779823.rettg)
	e2:SetOperation(c4779823.retop)
	c:RegisterEffect(e2)
	--discard deck
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(4779823,2))
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e3:SetCategory(CATEGORY_DECKDES)
	e3:SetCode(EVENT_PHASE+PHASE_END)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCountLimit(1)
	e3:SetCondition(c4779823.discon)
	e3:SetTarget(c4779823.distg)
	e3:SetOperation(c4779823.disop)
	c:RegisterEffect(e3)
end
function c4779823.rmcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,1000) end
	Duel.PayLPCost(tp,1000)
end
function c4779823.rmtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and chkc:IsAbleToRemove() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsAbleToRemove,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectTarget(tp,Card.IsAbleToRemove,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,1,0,0)
end
function c4779823.rmop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.Remove(tc,POS_FACEUP,REASON_EFFECT)
	end
end
function c4779823.filter(c)
	return c:IsSetCard(0x38) and c:IsType(TYPE_MONSTER) and c:IsAbleToDeck()
end
function c4779823.rettg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c4779823.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c4779823.filter,tp,LOCATION_GRAVE,0,1,e:GetHandler()) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local g=Duel.SelectTarget(tp,c4779823.filter,tp,LOCATION_GRAVE,0,1,99,e:GetHandler())
	Duel.SetOperationInfo(0,CATEGORY_TODECK,g,g:GetCount(),0,0)
	Duel.SetOperationInfo(0,CATEGORY_RECOVER,nil,0,tp,g:GetCount()*300)
end
function c4779823.retop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(Card.IsRelateToEffect,nil,e)
	local ct=Duel.SendtoDeck(g,nil,2,REASON_EFFECT)
	if ct>0 then
		Duel.Recover(tp,ct*300,REASON_EFFECT)
	end
end
function c4779823.discon(e,tp,eg,ep,ev,re,r,rp)
	return tp==Duel.GetTurnPlayer()
end
function c4779823.distg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DECKDES,nil,0,tp,3)
end
function c4779823.disop(e,tp,eg,ep,ev,re,r,rp)
	Duel.DiscardDeck(tp,3,REASON_EFFECT)
end
