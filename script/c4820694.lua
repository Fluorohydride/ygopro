--セイクリッド・テンペスト
function c4820694.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--damage
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(4820694,0))
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCountLimit(1,4820694)
	e3:SetCode(EVENT_PHASE+PHASE_END)
	e3:SetCondition(c4820694.con)
	e3:SetTarget(c4820694.lptg)
	e3:SetOperation(c4820694.lpop)
	c:RegisterEffect(e3)
	--damage
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(4820694,1))
	e3:SetCategory(CATEGORY_DAMAGE)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e3:SetProperty(EFFECT_FLAG_REPEAT)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCountLimit(1)
	e3:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e3:SetCondition(c4820694.con)
	e3:SetTarget(c4820694.mattg)
	e3:SetOperation(c4820694.matop)
	c:RegisterEffect(e3)
end
function c4820694.con(e,tp,eg,ep,ev,re,r,rp)
	return 	Duel.GetTurnPlayer()==tp 
end
function c4820694.tfilter(c)
	return c:IsFaceup() and c:IsType(TYPE_XYZ) and c:IsSetCard(0x53) and c:GetOverlayCount()>0
end
function c4820694.lptg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc==0 then return chkc:IsControler(tp) and chkc:IsLocation(LOCAIOTN_MZONE) and c4820694.tfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c4820694.cfilter,tp,LOCATION_MZONE,0,2,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	local g=Duel.SelectTarget(tp,c4820694.tfilter,tp,LOCATION_MZONE,0,2,2,nil)
end
function c4820694.lpop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local tc1=g:GetFirst()
	local tc2=g:GetNext()
	
	if tc1 and tc1:IsRelateToEffect(e) and tc2 and tc2:IsRelateToEffect(e) then
		tc1:RemoveOverlayCard(tp,tc1:GetOverlayCount(),tc1:GetOverlayCount(),REASON_EFFECT)
		tc2:RemoveOverlayCard(tp,tc2:GetOverlayCount(),tc2:GetOverlayCount(),REASON_EFFECT)
		Duel.SetLP(1-tp,math.floor(Duel.GetLP(1-tp)/2))
	end
end
function c4820694.filter(c,e,tp)
	return c:IsType(TYPE_XYZ) and c:IsSetCard(0x53) 
end
function c4820694.filter2(c)
	return c:IsFaceup() and c:IsSetCard(0x53) and c:IsType(TYPE_MONSTER)
end
function c4820694.mattg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then return Duel.IsExistingTarget(c4820694.filter,tp,LOCATION_MZONE,0,1,nil,e,tp) 
		and Duel.IsExistingTarget(c4820694.filter2,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	local g1=Duel.SelectTarget(tp,c4820694.filter,tp,LOCATION_MZONE,0,1,1,nil,e,tp)
	e:SetLabelObject(g1:GetFirst())
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	local g2=Duel.SelectTarget(tp,c4820694.filter2,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_LEAVE_GRAVE,g2,1,0,0)
end
function c4820694.matop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local sg=g:Filter(Card.IsRelateToEffect,nil,e)
	local tc=e:GetLabelObject()
	sg:RemoveCard(tc)
	local tc1=sg:GetFirst()
	if tc:IsRelateToEffect(e) and tc:IsFaceup() and tc1 and tc1:IsRelateToEffect(e) then
		Duel.Overlay(tc,tc1)
	end
end