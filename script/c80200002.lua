-- 	ＥＭトランポリンクス
function c80200002.initial_effect(c)
	--pendulum summon
	aux.AddPendulumProcedure(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--tohand
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(80200002,0))
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetType(EFFECT_TYPE_TRIGGER_O+EFFECT_TYPE_FIELD)
	e2:SetCategory(CATEGORY_TOHAND)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCountLimit(1,80200002)
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	e2:SetCondition(c80200002.thcon)
	e2:SetTarget(c80200002.thtg)
	e2:SetOperation(c80200002.thop)
	c:RegisterEffect(e2)
	--tohand 2
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(80200002,0))
	e3:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e3:SetType(EFFECT_TYPE_TRIGGER_O+EFFECT_TYPE_SINGLE)
	e3:SetCategory(CATEGORY_TOHAND)
	e3:SetCode(EVENT_SUMMON_SUCCESS)
	e3:SetTarget(c80200002.thtg)
	e3:SetOperation(c80200002.thop)
	c:RegisterEffect(e3)
end
function c80200002.cfilter(c,tp)
	return c:IsControler(tp) and c:GetSummonType()==SUMMON_TYPE_PENDULUM
end
function c80200002.thcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c80200002.cfilter,1,nil,tp)
end
function c80200002.filter(c)
	return (c:GetSequence()==6 or c:GetSequence()==7) and c:IsAbleToHand()
end
function c80200002.thtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_SZONE) and c80200002.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c80200002.filter,tp,LOCATION_SZONE,LOCATION_SZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
	local g=Duel.SelectTarget(tp,c80200002.filter,tp,LOCATION_SZONE,LOCATION_SZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,g:GetCount(),0,0)
end
function c80200002.thop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
	end
end