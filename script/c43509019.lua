--トゥーン・ディフェンス
function c43509019.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetTarget(c43509019.cbtg)
	e1:SetOperation(c43509019.cbop)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--change battle target
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(43509019,0))
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetCode(EVENT_ATTACK_ANNOUNCE)
	e2:SetRange(LOCATION_SZONE)
	e2:SetLabel(1)
	e2:SetCondition(c43509019.cbcon)
	e2:SetOperation(c43509019.cbop)
	c:RegisterEffect(e2)
end
function c43509019.cbtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local bt=Duel.GetAttackTarget()
	if Duel.CheckEvent(EVENT_ATTACK_ANNOUNCE) and bt and bt:IsFaceup() and bt:IsLevelBelow(4) and bt:IsType(TYPE_TOON)
		and bt:GetControler()==e:GetHandlerPlayer() and Duel.SelectYesNo(tp,aux.Stringid(43509019,1)) then
		e:SetLabel(1)
	else e:SetLabel(0) end
end
function c43509019.cbcon(e,tp,eg,ep,ev,re,r,rp)
	local bt=Duel.GetAttackTarget()
	return bt and bt:IsFaceup() and bt:IsLevelBelow(4) and bt:IsType(TYPE_TOON) and bt:GetControler()==e:GetHandlerPlayer()
end
function c43509019.cbop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==1 then
		Duel.ChangeAttackTarget(nil)
	end
end
