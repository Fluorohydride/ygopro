--デモンバルサム・シード
function c60406591.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c60406591.condition)
	e1:SetTarget(c60406591.target)
	e1:SetOperation(c60406591.activate)
	c:RegisterEffect(e1)
	if not c60406591.global_check then
		c60406591.global_check=true
		c60406591[0]=nil
		c60406591[1]=nil
		c60406591[2]=nil
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_BATTLE_DAMAGE)
		ge1:SetOperation(c60406591.checkop)
		Duel.RegisterEffect(ge1,0)
		local ge2=Effect.CreateEffect(c)
		ge2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge2:SetCode(EVENT_PHASE_START+PHASE_DRAW)
		ge2:SetOperation(c60406591.clear)
		Duel.RegisterEffect(ge2,0)
	end
end
function c60406591.checkop(e,tp,eg,ep,ev,re,r,rp)
	c60406591[0]=ep
	c60406591[1]=math.floor(ev/500)
	c60406591[2]=eg:GetFirst():GetBattleTarget()
end
function c60406591.clear(e,tp,eg,ep,ev,re,r,rp)
	c60406591[0]=nil
	c60406591[1]=nil
	c60406591[2]=nil
end
function c60406591.condition(e,tp,eg,ep,ev,re,r,rp)
	return c60406591[0]==tp and eg:GetCount()==1 and eg:GetFirst()==c60406591[2]
end
function c60406591.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return c60406591[1]>0 and Duel.GetLocationCount(tp,LOCATION_MZONE)>c60406591[1]
		and Duel.IsPlayerCanSpecialSummonMonster(tp,60406592,0,0x4011,100,100,1,RACE_PLANT,ATTRIBUTE_DARK) end
	Duel.SetOperationInfo(0,CATEGORY_TOKEN,nil,c60406591[1],0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,c60406591[1],0,0)
end
function c60406591.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<c60406591[1]
		or not Duel.IsPlayerCanSpecialSummonMonster(tp,60406592,0,0x4011,100,100,1,RACE_PLANT,ATTRIBUTE_DARK) then return end
	for i=1,c60406591[1] do
		local token=Duel.CreateToken(tp,60406592)
		Duel.SpecialSummonStep(token,0,tp,tp,false,false,POS_FACEUP)
	end
	Duel.SpecialSummonComplete()
end
