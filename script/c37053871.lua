--アストラルバリア
function c37053871.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_MAIN_END)
	e1:SetTarget(c37053871.atktg1)
	e1:SetOperation(c37053871.atkop)
	c:RegisterEffect(e1)
	--target change
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(37053871,1))
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetCode(EVENT_BE_BATTLE_TARGET)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCondition(c37053871.atkcon)
	e2:SetTarget(c37053871.atktg2)
	e2:SetOperation(c37053871.atkop)
	c:RegisterEffect(e2)
end
function c37053871.atkcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:GetFirst():IsControler(tp)
end
function c37053871.atktg1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	e:SetLabel(0)
	if Duel.CheckEvent(EVENT_BE_BATTLE_TARGET) and Duel.GetAttackTarget():IsControler(tp)
		and Duel.GetAttackTarget() and Duel.SelectYesNo(tp,aux.Stringid(37053871,0)) then
		e:SetLabel(1)
	end
end
function c37053871.atktg2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	e:SetLabel(1)
end
function c37053871.atkop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==0 or not e:GetHandler():IsRelateToEffect(e) then return end
	Duel.ChangeAttackTarget(nil)
end
