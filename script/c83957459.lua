--ライノタウルス
function c83957459.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_EXTRA_ATTACK)
	e1:SetCondition(c83957459.macon)
	e1:SetValue(1)
	c:RegisterEffect(e1)
	if not c83957459.global_check then
		c83957459.global_check=true
		c83957459[0]=0
		c83957459[1]=0
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_BATTLE_DESTROYING)
		ge1:SetOperation(c83957459.checkop)
		Duel.RegisterEffect(ge1,0)
		local ge2=Effect.CreateEffect(c)
		ge2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge2:SetCode(EVENT_PHASE_START+PHASE_DRAW)
		ge2:SetOperation(c83957459.clear)
		Duel.RegisterEffect(ge2,0)
	end
end
function c83957459.checkop(e,tp,eg,ep,ev,re,r,rp)
	local cp=eg:GetFirst():GetControler()
	c83957459[cp]=c83957459[cp]+1
end
function c83957459.clear(e,tp,eg,ep,ev,re,r,rp)
	c83957459[0]=0
	c83957459[1]=0
end
function c83957459.macon(e)
	return c83957459[e:GetHandlerPlayer()]>=2
end
