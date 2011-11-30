--ナチュル·スパイダーファング
function c25654671.initial_effect(c)
	--atk limit
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_CANNOT_ATTACK_ANNOUNCE)
	e1:SetCondition(c25654671.atkcon)
	c:RegisterEffect(e1)
	if not c25654671.global_check then
		c25654671.global_check=true
		c25654671[0]=false
		c25654671[1]=false
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_CHAIN_SOLVED)
		ge1:SetOperation(c25654671.chop)
		Duel.RegisterEffect(ge1,0)
		local ge2=Effect.CreateEffect(c)
		ge2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge2:SetCode(EVENT_PHASE_START+PHASE_DRAW)
		ge2:SetOperation(c25654671.clear)
		Duel.RegisterEffect(ge2,0)
	end
end
function c25654671.atkcon(e)
	return not c25654671[1-e:GetHandler():GetControler()]
end
function c25654671.chop(e,tp,eg,ep,ev,re,r,rp)
	c25654671[rp]=true
end
function c25654671.clear(e,tp,eg,ep,ev,re,r,rp)
	c25654671[0]=false
	c25654671[1]=false
end
