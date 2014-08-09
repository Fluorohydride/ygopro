--セイクリッド・テンペスト
function c4820694.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--lp
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(4820694,0))
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetCode(EVENT_PHASE+PHASE_END)
	e2:SetRange(LOCATION_SZONE)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCountLimit(1,4820694+EFFECT_COUNT_CODE_DUEL)
	e2:SetCondition(c4820694.condition)
	e2:SetTarget(c4820694.lptg)
	e2:SetOperation(c4820694.lpop)
	c:RegisterEffect(e2)
	--material
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(4820694,1))
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e3:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e3:SetRange(LOCATION_SZONE)
	e3:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e3:SetCountLimit(1)
	e3:SetCondition(c4820694.condition)
	e3:SetTarget(c4820694.mattg)
	e3:SetOperation(c4820694.matop)
	c:RegisterEffect(e3)
end
function c4820694.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp
end
function c4820694.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x53) and c:IsType(TYPE_XYZ) and c:GetOverlayCount()>0
end
function c4820694.lptg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c4820694.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c4820694.filter,tp,LOCATION_MZONE,0,2,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c4820694.filter,tp,LOCATION_MZONE,0,2,2,nil)
end
function c4820694.lpop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local tc1=g:GetFirst()
	local tc2=g:GetNext()
	if tc1:IsRelateToEffect(e) and tc2:IsRelateToEffect(e) then
		local og1=tc1:GetOverlayGroup()
		local og2=tc2:GetOverlayGroup()
		og1:Merge(og2)
		if Duel.SendtoGrave(og1,REASON_EFFECT)<og1:GetCount() then return end
		Duel.SetLP(1-tp,Duel.GetLP(1-tp)/2)
	end
end
function c4820694.tgfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x53) and c:IsType(TYPE_XYZ)
end
function c4820694.mfilter(c)
	return c:IsSetCard(0x53) and c:IsType(TYPE_MONSTER)
end
function c4820694.mattg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then return Duel.IsExistingTarget(c4820694.tgfilter,tp,LOCATION_MZONE,0,1,nil)
		and Duel.IsExistingTarget(c4820694.mfilter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	local g1=Duel.SelectTarget(tp,c4820694.tgfilter,tp,LOCATION_MZONE,0,1,1,nil)
	e:SetLabelObject(g1:GetFirst())
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_XMATERIAL)
	local g2=Duel.SelectTarget(tp,c4820694.mfilter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_LEAVE_GRAVE,g2,1,0,0)
end
function c4820694.matop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local tc=e:GetLabelObject()
	if tc:IsFacedown() or not tc:IsRelateToEffect(e) or tc:IsImmuneToEffect(e) then return end
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(Card.IsRelateToEffect,tc,e)
	if g:GetCount()>0 then
		Duel.Overlay(tc,g)
	end
end
