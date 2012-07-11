--ラッキー·チャンス！
function c96012004.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--coin
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(96012004,0))
	e2:SetCategory(CATEGORY_DRAW)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_QUICK_F)
	e2:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e2:SetCode(EVENT_CHAINING)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCondition(c96012004.coincon)
	e2:SetOperation(c96012004.coinop)
	c:RegisterEffect(e2)
end
function c96012004.coincon(e,tp,eg,ep,ev,re,r,rp)
	local ex,eg,et,cp,ct=Duel.GetOperationInfo(ev,CATEGORY_COIN)
	return ex and ct==1 and re:IsActiveType(TYPE_MONSTER)
end
function c96012004.coinop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_COIN)
	local res=1-Duel.SelectOption(tp,60,61)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetProperty(EFFECT_FLAG_DELAY)
	e1:SetCode(EVENT_TOSS_COIN)
	e1:SetCondition(c96012004.drcon)
	e1:SetOperation(c96012004.drop)
	e1:SetCountLimit(1)
	e1:SetReset(RESET_CHAIN)
	e1:SetLabelObject(re)
	e1:SetLabel(res)
	Duel.RegisterEffect(e1,tp)
end
function c96012004.drcon(e,tp,eg,ep,ev,re,r,rp)
	local res=Duel.GetCoinResult()
	return re==e:GetLabelObject() and res==e:GetLabel()
end
function c96012004.drop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_CARD,0,96012004)
	Duel.Draw(tp,1,REASON_EFFECT)
end
