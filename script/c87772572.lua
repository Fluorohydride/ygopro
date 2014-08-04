--Quantum Cat
function c87772572.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c87772572.target)
	e1:SetOperation(c87772572.activate)
	c:RegisterEffect(e1)
	--dummy effect
	local e2=Effect.CreateEffect(c)
	c:RegisterEffect(e2)
	e1:SetLabelObject(e2)
end
function c87772572.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsPlayerCanSpecialSummonMonster(tp,87772572,0,0x11,0,2200,4,nil,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,0)
	
	local att = 0x1
	local aat = 0x0
	while att<0x80 do
		if Duel.IsPlayerCanSpecialSummonMonster(tp,87772572,0,0x11,0,2200,4,nil,att) then
			aat=bit.bor(aat,att)
		end
		att=bit.lshift(att, 1) 
	end
	
	aat=Duel.AnnounceAttribute(tp,1,aat)
	e:SetLabel(aat)
	
	local rcc= 0x1
	local rc = 0x0
	
	while rcc<0x1000000 do
		if Duel.IsPlayerCanSpecialSummonMonster(tp,87772572,0,0x11,0,2200,4,rcc,nil) then
			rc=bit.bor(rc,rcc)
		end
		rcc=bit.lshift(rcc, 1) 
	end
	
	Duel.Hint(HINT_SELECTMSG,tp,563)
	rc=Duel.AnnounceRace(tp,1,rc)
	e:GetLabelObject():SetLabel(rc)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c87772572.activate(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) then return end
	local att=e:GetLabel()
	local race=e:GetLabelObject():GetLabel()
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0
		or not Duel.IsPlayerCanSpecialSummonMonster(tp,87772572,0,0x11,0,2200,4,race,att) then return end
	c:AddTrapMonsterAttribute(TYPE_NORMAL,att,race,4,0,2200)
	Duel.SpecialSummon(c,1,tp,tp,true,false,POS_FACEUP)
	c:TrapMonsterBlock()
end