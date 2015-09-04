--カース・オブ・スタチュー
function c3129635.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c3129635.target)
	e1:SetOperation(c3129635.activate)
	c:RegisterEffect(e1)
end
function c3129635.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and 
		Duel.IsPlayerCanSpecialSummonMonster(tp,3129635,0,0x21,1800,1000,4,RACE_ROCK,ATTRIBUTE_DARK) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c3129635.activate(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) then return end
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0
		or not Duel.IsPlayerCanSpecialSummonMonster(tp,3129635,0,0x21,1800,1000,4,RACE_ROCK,ATTRIBUTE_DARK) then return end
	c:AddTrapMonsterAttribute(TYPE_EFFECT,ATTRIBUTE_DARK,RACE_ROCK,4,1800,1000)
	Duel.SpecialSummon(c,0,tp,tp,true,false,POS_FACEUP)
	c:TrapMonsterBlock()
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(3129635,0))
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_BATTLED)
	e1:SetTarget(c3129635.destg)
	e1:SetOperation(c3129635.desop)
	e1:SetReset(RESET_EVENT+0x1fe0000)
	c:RegisterEffect(e1)
end
function c3129635.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	if chk==0 then
		if a:IsControler(tp) then return d and a~=e:GetHandler() and bit.band(a:GetOriginalType(),TYPE_TRAP)~=0
		else return d and d~=e:GetHandler() and bit.band(d:GetOriginalType(),TYPE_TRAP)~=0 end
	end
	if a:IsControler(tp) then
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,d,1,0,0)
		e:SetLabelObject(d)
	else
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,a,1,0,0)
		e:SetLabelObject(a)
	end
end
function c3129635.desop(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetLabelObject()
	if tc:IsRelateToBattle() then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
