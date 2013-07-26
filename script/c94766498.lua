--先史遺産アステカ・マスク・ゴーレム
function c94766498.initial_effect(c)
	c:SetUniqueOnField(1,0,94766498)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c94766498.hspcon)
	c:RegisterEffect(e1)
		if not c94766498.global_check then
		c94766498.global_check=true
		c94766498[0]=0
		c94766498[1]=0
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_CHAINING)
		ge1:SetOperation(c94766498.checkop1)
		Duel.RegisterEffect(ge1,0)
		local ge2=Effect.CreateEffect(c)
		ge2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge2:SetCode(EVENT_CHAIN_NEGATED)
		ge2:SetOperation(c94766498.checkop2)
		Duel.RegisterEffect(ge2,0)
		local ge3=Effect.CreateEffect(c)
		ge3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge3:SetCode(EVENT_PHASE_START+PHASE_DRAW)
		ge3:SetOperation(c94766498.clear)
		Duel.RegisterEffect(ge3,0)
	end
end
function c94766498.checkop1(e,tp,eg,ep,ev,re,r,rp)
	if re:IsHasType(EFFECT_TYPE_ACTIVATE) and re:GetHandler():IsSetCard(0x70) then
		c94766498[rp]=c94766498[rp]+1
	end
end
function c94766498.checkop2(e,tp,eg,ep,ev,re,r,rp)
	if re:IsHasType(EFFECT_TYPE_ACTIVATE) and re:GetHandler():IsSetCard(0x70) then
		c94766498[rp]=c94766498[rp]-1
	end
end
function c94766498.clear(e,tp,eg,ep,ev,re,r,rp)
	c94766498[0]=0
	c94766498[1]=0
end
function c94766498.hspcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	return c94766498[tp]>0 and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
end
