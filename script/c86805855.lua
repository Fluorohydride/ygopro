--闇魔界の竜騎士 ダークソード
function c86805855.initial_effect(c)
	--fusion material
	c:EnableReviveLimit()
	aux.AddFusionProcCode2(c,11321183,47415292,true,true)
	--remove
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(86805855,0))
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCode(EVENT_BATTLE_DAMAGE)
	e1:SetCondition(c86805855.rmcon)
	e1:SetTarget(c86805855.rmtg)
	e1:SetOperation(c86805855.rmop)
	c:RegisterEffect(e1)
end
function c86805855.rmcon(e,tp,eg,ep,ev,re,r,rp)
	return ep~=tp
end
function c86805855.filter(c)
	return c:IsType(TYPE_MONSTER) and c:IsAbleToRemove()
end
function c86805855.rmtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(1-tp) and chkc:IsLocation(LOCATION_GRAVE) and c86805855.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c86805855.filter,tp,0,LOCATION_GRAVE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectTarget(tp,c86805855.filter,tp,0,LOCATION_GRAVE,1,3,nil)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,g:GetCount(),0,0)
end
function c86805855.rmop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local sg=g:Filter(Card.IsRelateToEffect,nil,e)
	Duel.Remove(sg,POS_FACEUP,REASON_EFFECT)
end
