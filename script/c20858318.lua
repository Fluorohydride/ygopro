--必殺！黒蠍コンビネーション
function c20858318.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c20858318.con)
	e1:SetTarget(c20858318.tg)
	e1:SetOperation(c20858318.op)
	c:RegisterEffect(e1)
end
function c20858318.cfilter(c,code)
	return c:IsFaceup() and c:IsCode(code)
end
function c20858318.con(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c20858318.cfilter,tp,LOCATION_MZONE,0,1,nil,76922029)
		and Duel.IsExistingMatchingCard(c20858318.cfilter,tp,LOCATION_MZONE,0,1,nil,6967870)
		and Duel.IsExistingMatchingCard(c20858318.cfilter,tp,LOCATION_MZONE,0,1,nil,61587183)
		and Duel.IsExistingMatchingCard(c20858318.cfilter,tp,LOCATION_MZONE,0,1,nil,48768179)
		and Duel.IsExistingMatchingCard(c20858318.cfilter,tp,LOCATION_MZONE,0,1,nil,74153887)
end
function c20858318.tg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local g=Duel.GetFieldGroup(tp,LOCATION_MZONE,0)
	Duel.SetTargetCard(g)
end
function c20858318.op(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(Card.IsRelateToEffect,nil,e)
	local tc=g:GetFirst()
	while tc do
		tc:RegisterFlagEffect(20858318,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
		tc=g:GetNext()
	end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_DIRECT_ATTACK)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetTarget(c20858318.affected)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
	local e2=Effect.CreateEffect(e:GetHandler())
	e2:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e2:SetCode(EVENT_PRE_BATTLE_DAMAGE)
	e2:SetCondition(c20858318.rdcon)
	e2:SetOperation(c20858318.rdop)
	e2:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e2,tp)
end
function c20858318.affected(e,c)
	return c:GetFlagEffect(20858318)~=0
end
function c20858318.rdcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:GetFirst():GetFlagEffect(20858318)~=0
end
function c20858318.rdop(e,tp,eg,ep,ev,re,r,rp)
	Duel.ChangeBattleDamage(ep,400)
end
