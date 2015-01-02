--ラッキー・チャンス！
function c96012004.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c96012004.cointg)
	e1:SetOperation(c96012004.coinop)
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
	e2:SetLabel(1)
	c:RegisterEffect(e2)
end
function c96012004.coincon(e,tp,eg,ep,ev,re,r,rp)
	local ex,eg,et,cp,ct=Duel.GetOperationInfo(ev,CATEGORY_COIN)
	if ex and ct==1 and re:IsActiveType(TYPE_MONSTER) then
		e:SetLabelObject(re)
		return true
	else return false end
end
function c96012004.cointg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	e:SetLabel(0)
	local cc=Duel.GetCurrentChain()
	if cc==1 then return end
	local te=Duel.GetChainInfo(cc-1,CHAININFO_TRIGGERING_EFFECT)
	local ex,eg,et,cp,ct=Duel.GetOperationInfo(cc-1,CATEGORY_COIN)
	if ex and ct==1 and te:IsActiveType(TYPE_MONSTER) then
		e:SetLabel(1)
		e:SetLabelObject(te)
	end
end
function c96012004.coinop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if e:GetLabel()==0 or not c:IsRelateToEffect(e) then return end
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
	e1:SetLabelObject(e:GetLabelObject())
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
